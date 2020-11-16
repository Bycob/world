#include "GroundBiomes.h"

#include <vector>
#include <map>
#include <memory>
#include <list>

#include "world/core/WorldTypes.h"
#include "world/core/Chunk.h"
#include "world/math/Vector.h"
#include "world/math/BoundingBox.h"
#include "world/math/RandomHelper.h"
#include "world/assets/ColorOps.h"
#include "Terrain.h"
#include "MultilayerGroundTexture.h"
#include "TerrainOps.h"

namespace world {

WORLD_REGISTER_CHILD_CLASS(ITerrainWorker, GroundBiomes, "GroundBiomes")

class BiomeLayerInstance {
public:
    /// Id of the biome in the layers list
    u32 _layerId;
    /// Location of the seed in the world (absolute coordinates)
    vec3d _location;

    // for each point: 1 if present, 0 if absent.
    std::unique_ptr<Terrain> _distribution;

    BiomeLayerInstance(u32 biomeId, vec3d location)
            : _layerId(biomeId), _location(location), _distribution(nullptr) {}
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
    std::unique_ptr<MultilayerGroundTexture> _texturer;
    std::vector<BiomeType> _typeList;

    // Internal fields
    std::vector<BiomeLayer> _layers;
    std::map<vec2i, ChunkBiomes> _biomes;

    Terrain _holesBuffer;

    /// Currently generated surface. Used to compute how much layers
    double _coveredSurface = 0;

    GroundBiomesPrivate()
            : _texturer(std::make_unique<MultilayerGroundTexture>()),
              _holesBuffer(1) {}
};


template <> void write<BiomeType>(const BiomeType &bt, WorldFile &wf) {
    wf.addString("shader", bt._shader);
    wf.addChild("humidityPalette", bt._humidityPalette.serialize());
    wf.addStruct("dparams", bt._dparams);
    wf.addBool("allowHoles", bt._allowHoles);
}

template <> void read<BiomeType>(const WorldFile &wf, BiomeType &bt) {
    wf.readStringOpt("shader", bt._shader);
    if (wf.hasChild("humidityPalette"))
        bt._humidityPalette.read(wf.readChild("humidityPalette"));
    wf.readStructOpt("dparams", bt._dparams);
    wf.readBoolOpt("allowHoles", bt._allowHoles);
}

GroundBiomes::GroundBiomes(double biomeArea)
        : _internal(new GroundBiomesPrivate()), _rng(std::random_device()()),
          _biomeArea(biomeArea), _layerDensity(0.5),
          _temperatureBounds(-20, 40) {
    _chunkSize = std::sqrt(biomeArea) * 2;
    _chunkArea = _chunkSize * _chunkSize;

    // Init buffer
    _internal->_holesBuffer = Terrain(_distribResolution);
}

GroundBiomes::~GroundBiomes() { delete _internal; }

void GroundBiomes::addBiomeType(const BiomeType &type) {
    _internal->_typeList.push_back(type);
    _internal->_typeList.back()._id = _internal->_typeList.size() - 1;
}

void GroundBiomes::write(WorldFile &wf) const {
    wf.addChild("texturer", _internal->_texturer->serialize());
    wf.addArray("typeList");

    for (const auto &type : _internal->_typeList) {
        wf.addToArray("typeList", world::serialize(type));
    }
}

void GroundBiomes::read(const WorldFile &wf) {
    if (wf.hasChild("texturer"))
        _internal->_texturer->read(wf.readChild("texturer"));

    for (auto it = wf.readArray("typeList"); !it.end(); ++it) {
        _internal->_typeList.push_back(world::deserialize<BiomeType>(*it));
        _internal->_typeList.back()._id = _internal->_typeList.size() - 1;
    }
}

void GroundBiomes::processTerrain(Terrain &terrain) {
    _internal->_texturer->processTerrain(terrain);
}

void GroundBiomes::processTile(ITileContext &context) {
    auto c = context.getCoords();
    const ExplorationContext &ctx = context.getExplorationContext();

    vec3d terrainDims =
        context.getTile()._terrain.getBoundingBox().getDimensions();
    vec2i lower{(c._pos * terrainDims / _chunkSize).round()};
    vec2i upper{((c._pos + vec3d{1}) * terrainDims / _chunkSize).round()};

    const int dRes = _internal->_texturer->getDistributionResolution();
    std::map<int, Terrain> layers;

    for (int x = lower.x - 1; x <= upper.x + 1; ++x) {
        for (int y = lower.y - 1; y <= upper.y + 1; ++y) {
            generateBiomes({x, y}, ctx);
            ChunkBiomes &biomes = _internal->_biomes.at({x, y});

            for (auto &instance : biomes._instances) {
                auto it = layers.insert({instance._layerId, Terrain(dRes)});

                if (it.second) {
                    TerrainOps::fill(it.first->second, 0);
                }
                applyToDistribution({x, y}, instance, context,
                                    it.first->second);
            }
        }
    }

    // Setup texturer layers for this tile
    auto &texturerTile = _internal->_texturer->getTile(context.getCoords());

    // TODO reduce complexity
    for (size_t typeId = 0; typeId < _internal->_typeList.size(); ++typeId) {
        for (auto &entry : layers) {
            if (_internal->_layers[entry.first]._type == typeId) {
                texturerTile._layerIds.emplace_back(entry.first);
                texturerTile._distributions.emplace_back(
                    std::move(entry.second));
            }
        }
    }

    _internal->_texturer->processTile(context);
}

void GroundBiomes::collectTile(ICollector &collector, ITileContext &context) {
    _internal->_texturer->collectTile(collector, context);
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

void GroundBiomes::applyToDistribution(const vec2i &biomeCoords,
                                       BiomeLayerInstance &instance,
                                       ITileContext &context,
                                       Terrain &distribution) {
    int res = distribution.getResolution();

    for (int y = 0; y < res; ++y) {
        for (int x = 0; x < res; ++x) {
            double xd = double(x) / (res - 1), yd = double(y) / (res - 1);
            distribution(x, y) +=
                getBiomeValue(biomeCoords, instance, context, {xd, yd});
        }
    }
}

double GroundBiomes::getBiomeValue(const vec2i &biomeCoords,
                                   const BiomeLayerInstance &instance,
                                   const ITileContext &context,
                                   const vec2d &terrainPos) {

    vec2i terrainCoords{context.getCoords()._pos};
    double terrainWidth =
        context.getTile()._terrain.getBoundingBox().getDimensions().x;
    vec2d globalLoc = (terrainCoords + terrainPos) * terrainWidth;
    vec2d loc = (globalLoc / _chunkSize) - (biomeCoords - vec2d{1.5});
    loc /= 3;

    return instance._distribution->getCubicHeight(loc.x, loc.y);
}

void GroundBiomes::generateLayersAsNeeded(ChunkBiomes &chunk) {
    _internal->_coveredSurface += _chunkArea;
    std::vector<size_t> typeCounts(_internal->_typeList.size(), 0);

    for (const BiomeLayer &layer : _internal->_layers) {
        typeCounts[layer._type]++;
    }

    for (size_t i = 0; i < _internal->_typeList.size(); ++i) {
        if (typeCounts[i] == 0) {
            for (int j = 0; j < _minBiomeCount; ++j) {
                double u = double(j * 2 + 1) / (_minBiomeCount * 2);
                auto tb = _temperatureBounds;
                createLayer(i, mix(tb.x, tb.y, u), u);
            }
        }
    }
}

BiomeLayer &GroundBiomes::createLayer(int typeId, double temperature,
                                      double humidity) {
    const BiomeType &type = _internal->_typeList[typeId];

    _internal->_layers.emplace_back();
    BiomeLayer &layer = _internal->_layers.back();
    layer._id = _internal->_layers.size() - 1;
    layer._type = typeId;
    layer._shader = type._shader;

    // pick temperature and humidity
    const vec2d &tb = _temperatureBounds;
    std::normal_distribution<double> tempDistrib(0, (tb.y - tb.x) * 0.1);
    double tempDiff = abs(tempDistrib(_rng));
    layer._temperature = {clamp(temperature - tempDiff, tb.x, tb.y),
                          clamp(temperature + tempDiff, tb.x, tb.y)};

    std::normal_distribution<double> humidDistrib(0, 0.1);
    double humidDiff = abs(humidDistrib(_rng));
    layer._humidity = {clamp(humidity - humidDiff, 0, 1),
                       clamp(humidity + humidDiff, 0, 1)};

    // pick color (with palette)
    layer._colors.push_back(type._humidityPalette.getColor(humidity));

    // pick style
    std::uniform_real_distribution<double> d(0, 1);
    layer._style = {d(_rng), d(_rng), d(_rng)};

    // Add to multilayerground textures layers & texture generator
    _internal->_texturer->addLayer(type._dparams);
    auto *texGen = dynamic_cast<IBiomeTextureGenerator *>(
        &_internal->_texturer->getTextureProvider());
    if (texGen) {
        texGen->addLayer(layer);
    } else {
        throw std::runtime_error(
            "Layer could not be added because no texture generator was found");
    }
    return layer;
}

int GroundBiomes::selectLayer(int type, double temperature, double humidity) {
    int layerId = -1;
    std::normal_distribution<double> jitter(0, 0.1);
    humidity += jitter(_rng);
    // TODO not check all the layer to select one (dichotomie?)

    for (auto &layer : _internal->_layers) {
        if (layer._type != type)
            continue;

        if (layer._humidity.x < humidity || layerId == -1) {
            layerId = layer._id;
        }
    }

    if (layerId == -1)
        throw std::runtime_error(
            "No layer was generated of the required type: " +
            std::to_string(type));

    return layerId;
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

    // TODO double check that the list is sorted in the correct order
    std::sort(allSeeds.begin(), allSeeds.end(),
              [this](BiomeLayerInstance *b1, BiomeLayerInstance *b2) {
                  return layerType(b1->_layerId) < layerType(b2->_layerId);
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
                   layerType((*allSeedIt)->_layerId) == type._id) {
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
            TerrainOps::fill(_internal->_holesBuffer, 0);

            for (auto it = allSeedBegin; it != allSeedIt; ++it) {
                BiomeLayerInstance &seed = *(*it);
                vec2i seedChunkPos{
                    (vec2d(seed._location) / _chunkSize + vec2d{0.5}).floor()};

                if (!seed._distribution) {
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
    generateLayersAsNeeded(chunk);

    std::uniform_real_distribution<double> humidity(0, 1);

    for (const BiomeType &type : _internal->_typeList) {
        int biomeCount = randRound(_rng, biomeCountd);

        for (int i = 0; i < biomeCount; i++) {
            vec3d localPos{offsetDistrib(_rng), offsetDistrib(_rng), 0};
            vec3d seedPos =
                (vec3d(chunkPos) - vec3d(0.5, 0.5, 0)) * _chunkSize + localPos;

            // Select layer based on humidity and temperature
            // TODO use real temperature and humidity
            int selectedLayer = selectLayer(type._id, 0, humidity(_rng));
            chunk._instances.emplace_back(selectedLayer, seedPos);
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

u32 GroundBiomes::layerType(u32 layerId) const {
    return _internal->_layers[layerId]._type;
}
} // namespace world