#ifndef WORLD_GROUNDBIOMES_H
#define WORLD_GROUNDBIOMES_H

#include "world/core/WorldConfig.h"

#include "world/math/Perlin.h"
#include "ITerrainWorker.h"

namespace world {

class BiomeType {
public:
    std::string _shader;
    // palettes

    u32 _id;

    /** If _allowHoles = false this type of layer must exist everywhere
     * on the terrain. */
    bool _allowHoles = false;

    BiomeType(std::string shader) : _shader(std::move(shader)) {}
};

class GroundBiomesPrivate;

class BiomeLayer;
class ChunkBiomes;
class BiomeLayerInstance;

class WORLDAPI_EXPORT GroundBiomes : public ITerrainWorker {
    WORLD_WRITE_SUBCLASS_METHOD
public:
    GroundBiomes(double biomeArea = 100e6);

    ~GroundBiomes() override;

    void addBiomeType(const BiomeType &type);

    void processTerrain(Terrain &terrain) override;

    void processTile(ITileContext &context) override;

    void flush() override;

    void write(WorldFile &wf) const override;

    void read(const WorldFile &wf) override;

    /** Create an image to visualize the biomes of a particular BiomeType. */
    void exportZones(Image &output, const BoundingBox &bbox, u32 biomeType);

private:
    GroundBiomesPrivate *_internal;

    std::mt19937_64 _rng;
    /// Average biome area in m^2
    double _biomeArea;
    /// Density of layers, ie for a given surface, how many different
    /// possible layers are there.
    double _layerDensity;

    /// Width of a biomechunk in m
    double _chunkSize;
    /// Area of a biomechunk in m^2 (typically, _chunkSize^2)
    double _chunkArea;

    /// Resolution of the distribution buffer for each layer (for one tile only)
    u32 _distribResolution = 64;


    BiomeLayer createLayer(int type);


    void generateBiomes(const vec2i &chunkPos, const ExplorationContext &ctx);

    /** Add seeds to the chunk
     * @param chunkPos coordinates of the chunk in the GroundBiomes system. */
    void seedChunk(const vec2i &chunkPos, ChunkBiomes &chunk);

    /** Create a first zone for the seed, without taking other seeds into
     * account */
    void setupZone(BiomeLayerInstance &seed, Perlin &perlin,
                   const vec2i &chunkPos);

    u32 zoneBuffersResolution() const;
};

} // namespace world


#endif // WORLD_GROUNDBIOMES_H
