#include "GroundBiomes.h"

#include <vector>
#include <map>
#include <memory>
#include <world/assets/ColorOps.h>

#include "world/core/WorldTypes.h"
#include "world/core/Chunk.h"
#include "world/math/Vector.h"
#include "world/math/BoundingBox.h"
#include "world/math/RandomHelper.h"
#include "Terrain.h"

namespace world {

WORLD_REGISTER_CHILD_CLASS(ITerrainWorker, GroundBiomes, "GroundBiomes")

class BiomeLayer {
public:
    // Image _img;

    std::vector<Color4d> _colors;
    /// style embedding (the style is determined by the shader
    /// relatively to the embedding)
    vec3d _style;
    /// To identify similar layers
    int _type;

    vec2d _humidity;
    vec2d _temperature;
};

class BiomeLayerInstance {
public:
    /// Id of the biome in the layers list
    u32 _biomeId;
    /// Location of the seed in the world (absolute coordinates)
    vec3d _location;

    BoundingBox _bbox;
    // 1 if present, 0 if absent.
    std::unique_ptr<Terrain> _distribution;

    BiomeLayerInstance(u32 biomeId, vec3d location)
            : _biomeId(biomeId), _location(location) {}
};

class ChunkBiomes {
public:
    std::vector<BiomeLayerInstance> _instances;
    /// If all the instance are generated in the chunk.
    /// The instances can be created but not generated yet.
    bool _generated = false;

    /// To keep track of how many layers there are in each place.
    std::unique_ptr<Terrain> _distribution;
};

class GroundBiomesPrivate {
public:
    std::vector<BiomeType> _typeList;

    // Internal fields
    std::vector<BiomeLayer> _layers;
    std::map<vec2i, ChunkBiomes> _biomes;

    Terrain _holesBuffer;

    GroundBiomesPrivate() : _holesBuffer(1) {}
};

GroundBiomes::GroundBiomes(double biomeArea)
        : _internal(new GroundBiomesPrivate()), _rng(std::random_device()()),
          _biomeArea(biomeArea), _layerDensity(0.5) {
    _chunkSize = std::sqrt(biomeArea) * 2;
    _chunkArea = _chunkSize * _chunkSize;

    // Init buffer
    _internal->_holesBuffer = Terrain(_distribResolution);

    // Testing purpose (TODO remove)
    addBiomeType({"texture-grass.frag"});
}

GroundBiomes::~GroundBiomes() { delete _internal; }

void GroundBiomes::addBiomeType(const BiomeType &type) {
    _internal->_typeList.push_back(type);
    _internal->_typeList.back()._id = _internal->_typeList.size() - 1;
}

void GroundBiomes::write(WorldFile &wf) const {}

void GroundBiomes::read(const WorldFile &wf) {}

void GroundBiomes::processTerrain(Terrain &terrain) {}

void GroundBiomes::processTile(ITileContext &context) {
    auto c = context.getCoords();
    if (c._lod != 0) {
        return;
    }

    // TODO get real ctx
    ExplorationContext ctx = ExplorationContext::getDefault();

    vec3d terrainDims =
        context.getTile()._terrain.getBoundingBox().getDimensions();
    vec2i lower{(c._pos * terrainDims / _chunkSize).round()};
    vec2i upper{((c._pos + vec3d{1}) * terrainDims / _chunkSize).round()};

    for (int x = lower.x; x <= upper.x; ++x) {
        for (int y = lower.y; y <= upper.y; ++y) {
            generateBiomes({x, y}, ctx);
        }
    }
}

void GroundBiomes::flush() {}

void GroundBiomes::exportZones(Image &output, const BoundingBox &bbox,
                               u32 biomeType) {
    if (biomeType >= _internal->_typeList.size()) {
        throw std::runtime_error("Wrong biome type");
    }
    std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<double> distrib(0, 1);

    int bufRes = int(zoneBuffersResolution());

    vec2d offset = vec2d(bbox.getLowerBound());
    vec2d pixelSize(bbox.getDimensions() /
                    vec3d(output.width(), output.height(), 1));

    vec2i lower = (vec2d(bbox.getLowerBound()) / _chunkSize).round();
    vec2i upper = (vec2d(bbox.getUpperBound()) / _chunkSize).round();

    for (int x = lower.x; x <= upper.x; ++x) {
        for (int y = lower.y; y <= upper.y; ++y) {
            auto it = _internal->_biomes.find({x, y});

            if (it == _internal->_biomes.end()) {
                continue;
            }

            ChunkBiomes &chunkBiomes = it->second;

            if (!chunkBiomes._generated) {
                continue;
            }

            vec2i chunkPos{x, y};
            // TODO duplicated computing of offset with "setupZone"
            vec2d chunkOffset = (vec2d{chunkPos} - vec2d{1.5}) * _chunkSize;
            double chunkPixelSize = _chunkSize * 3. / bufRes;

            for (auto &instance : chunkBiomes._instances) {
                Color4d seedColor =
                    ColorOps::fromHSV(distrib(rng), 0.8, 0.8, 0.5);

                for (u32 imgX = 0; imgX < output.width(); ++imgX) {
                    for (u32 imgY = 0; imgY < output.height(); ++imgY) {
                        vec2u imgPos{imgX, imgY};
                        vec2d realPos = offset + vec2d(imgPos) * pixelSize;
                        vec2i bufPos =
                            vec2i((realPos - chunkOffset) / chunkPixelSize);

                        if (bufPos.x < 0 || bufPos.x >= bufRes ||
                            bufPos.y < 0 || bufPos.y >= bufRes)
                            continue;

                        Color4d baseColor = output.rgba(imgX, imgY);
                        seedColor._a =
                            0.5 * (*instance._distribution)(bufPos.x, bufPos.y);
                        output.rgba(imgX, imgY) =
                            ColorOps::superpose(baseColor, seedColor);
                    }
                }
            }
        }
    }
}

BiomeLayer GroundBiomes::createLayer(int type) {
    BiomeLayer layer;
    layer._type = type;
    // pick color (with palette)

    // pick style
    std::uniform_real_distribution<double> d(0, 1);
    layer._style = {d(_rng), d(_rng), d(_rng)};
    return layer;
}

void GroundBiomes::generateBiomes(const vec2i &chunkPos,
                                  const ExplorationContext &ctx) {
    auto &env = ctx.getEnvironment();

    std::vector<BiomeLayerInstance *> allSeeds;

    // Add the seeds on all chunk around
    for (int y = chunkPos.y - 1; y <= chunkPos.y + 1; ++y) {
        for (int x = chunkPos.x - 1; x <= chunkPos.x + 1; ++x) {
            vec2i pos{x, y};
            auto res = _internal->_biomes.insert({pos, ChunkBiomes{}});

            if (res.second) {
                seedChunk(pos, res.first->second);
            }

            for (auto &seed : res.first->second._instances) {
                allSeeds.push_back(&seed);
            }
        }
    }

    // TODO double check that the list is sorted in the good order
    std::sort(allSeeds.begin(), allSeeds.end(),
              [](BiomeLayerInstance *b1, BiomeLayerInstance *b2) {
                  return b1->_biomeId > b2->_biomeId;
              });

    // Each "type" requires to have at least 1 layer everywhere
    ChunkBiomes &chunkBiomes = _internal->_biomes[chunkPos];
    int tileRes = int(_distribResolution);
    double chunkPixSize = _chunkSize / tileRes;
    vec2d chunkOffset = chunkPos * _chunkSize;

    if (!chunkBiomes._generated) {
        auto allSeedIt = allSeeds.begin();

        for (auto &type : _internal->_typeList) {
            Perlin perlin;

            // Get all the seeds of the same type
            auto allSeedBegin = allSeedIt;
            u32 seedCount = 0;

            while (allSeedIt != allSeeds.end() &&
                   (*allSeedIt)->_biomeId == type._id) {
                ++allSeedIt;
                ++seedCount;
            }

            // This call only compute biome distribution for the current chunk
            // (It does not change the distributions of seeds in other chunks,
            // but it can modify nearby chunks seeds distribution if their zone
            // overlap with this chunk)

            // I - Map the holes in the chunk
            // Create holes if seed is not adapted to its environment

            // Reset the holeBuffer
            for (int y = 0; y < tileRes; ++y) {
                for (int x = 0; x < tileRes; ++x) {
                    _internal->_holesBuffer(x, y) = 0;
                }
            }

            for (auto it = allSeedBegin; it != allSeedIt; ++it) {
                BiomeLayerInstance &seed = *(*it);
                vec2i seedChunkPos{
                    (vec2d(seed._location) / _chunkSize + vec2d{0.5}).floor()};

                if (seed._distribution == nullptr) {
                    setupZone(seed, perlin, seedChunkPos);
                }

                Terrain &seedDistrib = *seed._distribution;
                vec2i seedOffset =
                    (chunkPos - seedChunkPos + vec2i{1}) * tileRes;

                for (int y = 0; y < tileRes; ++y) {
                    for (int x = 0; x < tileRes; ++x) {
                        int seedX = x + seedOffset.x, seedY = y + seedOffset.y;
                        // TODO Environment penalty
                        seedDistrib(seedX, seedY) *= 1;

                        // Compute holes
                        _internal->_holesBuffer(x, y) +=
                            seedDistrib(seedX, seedY);
                    }
                }
            }

            // II - for each point in the holes, compute a coefficient based on
            // the exponential of the distance with a environmental penalty and
            // a perlin disturbance
            //
            // III - Choose the seed with the highest coefficient and assign it
            // to 1 (or softmax) to fill the hole

            if (!type._allowHoles) {
                std::vector<double> coefs(seedCount, 0);
                const double temperature = 100;

                for (int y = 0; y < tileRes; ++y) {
                    for (int x = 0; x < tileRes; ++x) {
                        double holeValue = _internal->_holesBuffer(x, y);

                        if (holeValue < 1) {
                            double coefsSum = 0;

                            // Compute coefs
                            int i = 0;
                            for (auto it = allSeedBegin; it != allSeedIt;
                                 ++it) {
                                vec2d seedPos((*it)->_location);
                                vec2d currentPos{chunkOffset +
                                                 vec2i{x, y} * chunkPixSize};

                                double d = seedPos.length(currentPos);
                                double coef =
                                    exp(-d * temperature / _chunkSize);
                                coefsSum += coef;
                                coefs[i] = coef;
                                ++i;
                            }

                            double coefMult = (1 - holeValue) / coefsSum;

                            // Apply coefs to every seed
                            i = 0;
                            for (auto it = allSeedBegin; it != allSeedIt;
                                 ++it) {
                                Terrain &seedDistrib = *(*it)->_distribution;
                                // Same as above, put in a common method or
                                // whatever?
                                vec2i seedChunkPos{
                                    (vec2d((*it)->_location) / _chunkSize +
                                     vec2d{0.5})
                                        .floor()};
                                vec2i seedOffset =
                                    (chunkPos - seedChunkPos + vec2i{1}) *
                                    tileRes;
                                int seedX = x + seedOffset.x,
                                    seedY = y + seedOffset.y;

                                seedDistrib(seedX, seedY) +=
                                    coefs[i] * coefMult;

                                ++i;
                            }
                        } // if (holeValue < 1)
                    }
                }
            } // if (!type._allowHoles) {
        }

        chunkBiomes._generated = true;
    }
}

void GroundBiomes::seedChunk(const vec2i &chunkPos, ChunkBiomes &chunk) {
    std::uniform_real_distribution<double> offsetDistrib(0, _chunkSize);
    double biomeCountd = _chunkArea / _biomeArea * _layerDensity;

    for (const BiomeType &type : _internal->_typeList) {
        int biomeCount = randRound(_rng, biomeCountd);

        for (int i = 0; i < biomeCount; i++) {
            vec3d localPos{offsetDistrib(_rng), offsetDistrib(_rng), 0};
            vec3d seedPos =
                (vec3d(chunkPos) - vec3d(0.5, 0.5, 0)) * _chunkSize + localPos;

            // Select layer based on humidity and temperature
            chunk._instances.emplace_back(type._id, seedPos);
        }
    }
}

void GroundBiomes::setupZone(BiomeLayerInstance &seed, Perlin &perlin,
                             const vec2i &chunkPos) {
    int bufRes = int(zoneBuffersResolution());
    seed._distribution = std::make_unique<Terrain>(bufRes);
    vec2d offset = (vec2d{chunkPos} - vec2d{1.5}) * _chunkSize;
    double pixelSize = _chunkSize * 3. / bufRes;
    vec2d seedPos(seed._location);
    // we use squared distance for speed
    double boundDist = _chunkSize * _chunkSize / 4;
    const double noiseFactor = 2; // make this a parameter

    // Generation of perlin
    PerlinInfo pi;
    int freq = 4;
    pi.octaves = 3;
    pi.reference = 0;
    pi.frequency = freq;
    pi.persistence = 0.9;
    pi.offsetX = chunkPos.x * freq;
    pi.offsetY = chunkPos.y * freq;
    auto noise = perlin.generatePerlinNoise2D(bufRes, pi);

    // Creation of zones throught warping
    for (u32 y = 0; y < bufRes; ++y) {
        for (u32 x = 0; x < bufRes; ++x) {
            vec2d pos = offset + vec2d(x, y) * pixelSize;
            double d = seedPos.squaredLength(pos);
            double n = (1 + (noise(x, y) - 0.5) * noiseFactor);
            (*seed._distribution)(x, y) =
                smoothstep(boundDist, boundDist * 0.5, d * n);
        }
    }

    // TODO temporary
    // export the zone to an image to visualize what shape it has
    /*seed._distribution->createImage().write(
        "zones/zone" + std::to_string(chunkPos.x) + "_" +
        std::to_string(chunkPos.y) + ".png");*/
}

u32 GroundBiomes::zoneBuffersResolution() const {
    return _distribResolution * 3;
}
} // namespace world