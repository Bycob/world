#ifndef WORLD_GROUNDBIOMES_H
#define WORLD_GROUNDBIOMES_H

#include "world/core/WorldConfig.h"

#include "world/math/Perlin.h"
#include "world/core/ColorPalette.h"
#include "ITerrainWorker.h"
#include "DistributionParams.h"

namespace world {

struct WORLDAPI_EXPORT BiomeType {
    /// Id of the biome type. A biome with a low type id will be drawn
    /// first on the texture.
    u32 _id;
    std::string _shader;
    ColorPalette _humidityPalette;
    DistributionParams _dparams;

    /** If _allowHoles = false this type of layer must exist everywhere
     * on the terrain. */
    bool _allowHoles = false;


    BiomeType()
            : _id(0), _shader(""), _humidityPalette({}, {}, {}, {}),
              _dparams() {}

    BiomeType(std::string shader, ColorPalette palette,
              DistributionParams dparams)
            : _id(0), _shader(std::move(shader)), _humidityPalette(palette),
              _dparams(dparams) {}
};

struct WORLDAPI_EXPORT BiomeLayer {
    /// Id of the layer
    int _id;
    /// To identify similar layers
    int _type;

    std::string _shader;

    std::vector<Color4d> _colors;
    /// style embedding (the style is determined by the shader
    /// relatively to the embedding)
    vec3d _style;

    vec2d _humidity;
    vec2d _temperature;
};

/** Generator for biome textures. Can generate terrain repetable
 * textures from BiomeLayer description. */
class WORLDAPI_EXPORT IBiomeTextureGenerator {
public:
    virtual ~IBiomeTextureGenerator() = default;

    virtual void addLayer(const BiomeLayer &layer) = 0;
};

class GroundBiomesPrivate;

class BiomeLayer;
class ChunkBiomes;
class BiomeLayerInstance;

class WORLDAPI_EXPORT GroundBiomes : public ITerrainWorker {
    WORLD_WRITE_SUBCLASS_METHOD
public:
    explicit GroundBiomes(double biomeArea = 100e6);

    ~GroundBiomes() override;

    void addBiomeType(const BiomeType &type);

    void processTerrain(Terrain &terrain) override;

    void processTile(ITileContext &context) override;

    void collectTile(ICollector &collector, ITileContext &context) override;

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
    /// Minimum and maximum possible value of temperature.
    vec2d _temperatureBounds;
    /// Minimum biome layer count
    int _minBiomeCount = 3;

    /// Width of a biomechunk in m
    double _chunkSize;
    /// Area of a biomechunk in m^2 (typically, _chunkSize^2)
    double _chunkArea;

    /// Resolution of the distribution buffer for each layer (for one tile only)
    u32 _distribResolution = 64;


    void applyToDistribution(const vec2i &biomeCoords,
                             BiomeLayerInstance &instance,
                             ITileContext &context, Terrain &distribution);

    /** Get the biome value for specified biome at the specified position
     * in the terrain with coordinates `tc`.
     * \param coords Coordinates of biome. */
    double getBiomeValue(const vec2i &biomeCoords,
                         const BiomeLayerInstance &instance,
                         const ITileContext &context, const vec2d &terrainPos);

    /** Generate layers that are required for the additionnal surface added
     * by this chunk. */
    void generateLayersAsNeeded(ChunkBiomes &chunk);

    BiomeLayer &createLayer(int type, double temperature, double humidity);

    int selectLayer(int type, double temperature, double humidity);

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
