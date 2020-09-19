#include "GroundBiomes.h"

#include <vector>
#include <map>
#include <memory>

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
};

GroundBiomes::GroundBiomes(double biomeArea)
        : _internal(new GroundBiomesPrivate()), _rng(std::random_device()()),
          _biomeArea(biomeArea), _layerDensity(1. / 6.e6) {
    _chunkSize = std::sqrt(biomeArea) * 2;
    _chunkArea = _chunkSize * _chunkSize;

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
    auto &env = ctx.getEnvironment();

    vec2i chunkPos{
        (c._pos * context.getTile()._terrain.getBoundingBox().getDimensions() /
         _chunkSize)
            .round()};
    std::vector<BiomeLayerInstance *> allSeeds;
    bool needGeneration = true;

    // Add the seeds on all chunk around
    for (int x = chunkPos.x - 1; x <= chunkPos.x + 1; ++x) {
        for (int y = chunkPos.y - 1; y <= chunkPos.y + 1; ++y) {
            vec2i pos{x, y};
            auto res = _internal->_biomes.insert({pos, ChunkBiomes{}});

            if (res.second) {
                seedChunk(pos, res.first->second);
            } else if (pos == chunkPos && res.first->second._generated) {
                needGeneration = false;
            }

            for (auto &seed : res.first->second._instances) {
                allSeeds.push_back(&seed);
            }
        }
    }

    // TODO check that the list is sorted in the good order
    std::sort(allSeeds.begin(), allSeeds.end(),
              [](BiomeLayerInstance *b1, BiomeLayerInstance *b2) {
                  return b1->_biomeId > b2->_biomeId;
              });

    // Create two noise map: one is completely independent from temperature /
    // humidity and the other is directly linked. By ponderation between the 2,
    // we can have biomes that follow geography, but not always

    // Each "type" requires to have at least 1 layer everywhere
    ChunkBiomes &chunkBiomes = _internal->_biomes[chunkPos];

    if (needGeneration) {
        auto currentSeedIt = chunkBiomes._instances.begin();
        auto allSeedIt = allSeeds.begin();

        for (auto &type : _internal->_typeList) {
            Perlin perlin;

            // Get bounds of seed of the same type
            auto allSeedBegin = allSeedIt;

            while (allSeedIt != allSeeds.end() &&
                   (*allSeedIt)->_biomeId == type._id) {
                ++allSeedIt;
            }

            // Process all new seed of this type
            // Compute a zone for each seed using perlin + warping (with
            // weights) Fill the holes by making zone fights Where they overlap,
            // fight as well but both can stay (and will still overlap)
            while (currentSeedIt != chunkBiomes._instances.end() &&
                   currentSeedIt->_biomeId == type._id) {
                setupZone(*currentSeedIt, perlin, chunkPos);

                for (auto it = allSeedBegin; it != allSeedIt; ++it) {
                }

                ++currentSeedIt;
            }
        }

        chunkBiomes._generated = true;
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

void GroundBiomes::seedChunk(const vec2i &chunkPos, ChunkBiomes &chunk) {
    std::uniform_real_distribution<double> offsetDistrib(0, _chunkSize);
    double biomeCountd = _chunkArea / _biomeArea;

    for (const BiomeType &type : _internal->_typeList) {
        int biomeCount = randRound(_rng, biomeCountd);

        for (int i = 0; i < biomeCount; i++) {
            vec3d localPos{offsetDistrib(_rng), offsetDistrib(_rng), 0};
            vec3d seedPos = (vec3d(chunkPos) - vec3d(0.5, 0.5, 0)) * _chunkSize + localPos;

            // Select layer based on humidity and temperature
            chunk._instances.emplace_back(type._id, seedPos);
        }
    }
}

void GroundBiomes::setupZone(BiomeLayerInstance &seed, Perlin &perlin,
                             const vec2i &chunkPos) {
    seed._distribution = std::make_unique<Terrain>(_distribResolution);
    vec2d offset = (vec2d{chunkPos} - vec2d{1.5, 1.5}) * _chunkSize;
    double pixelSpace = _chunkSize * 3. / _distribResolution;
    vec2d seedPos(seed._location);
    // we use squared distance for speed
    double boundDist = _chunkSize * _chunkSize;
    const double noiseFactor = 2;

    // Generation of perlin
    PerlinInfo pi;
    int freq = 4;
    pi.octaves = 3;
    pi.reference = 0;
    pi.frequency = freq;
    pi.persistence = 0.9;
    pi.offsetX = chunkPos.x * freq;
    pi.offsetY = chunkPos.y * freq;
    auto noise = perlin.generatePerlinNoise2D(_distribResolution, pi);

    // Creation of zones throught warping
    for (u32 y = 0; y < _distribResolution; ++y) {
        for (u32 x = 0; x < _distribResolution; ++x) {
            vec2d pos = offset + vec2d(x, y) * pixelSpace;
            double d = seedPos.squaredLength(pos);
            double n = (1 + (noise(x, y) - 0.5) * noiseFactor);
            (*seed._distribution)(x, y) =
                smoothstep(boundDist, boundDist * 0.5, d * n);
        }
    }

    // TODO temporary
    if (chunkPos == vec2d{}) {
        // export the zone to an image to visualize what shape it has
        seed._distribution->createImage().write("zone.png");
    }
}
} // namespace world