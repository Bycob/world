#ifndef VKWORLD_MULTILAYER_GROUND_TEXTURE_H
#define VKWORLD_MULTILAYER_GROUND_TEXTURE_H

#include "VkWorldConfig.h"

#include <random>

#include <world/terrain/ITerrainWorker.h>

namespace world {

class MultilayerGroundTexturePrivate;

struct VKWORLD_EXPORT DistributionParams {
    float ha;
    float hb;
    float hc;
    float hd;
    float dha;
    float dhb;
    float dhc;
    float dhd;
    float hmin;
    float hmax;
    float dhmin;
    float dhmax;
    float threshold;
    float slopeFactor;
};

class VKWORLD_EXPORT MultilayerGroundTexture : public ITerrainWorker {
public:
    MultilayerGroundTexture();

    ~MultilayerGroundTexture() override;

    /** Add layers corresponding to a world similar to the Earth. */
    void addDefaultLayers();

    void addLayer(const DistributionParams &distribution,
                  const std::string &textureShader);

    void processTerrain(Terrain &terrain) override;

    void processTile(ITileContext &context) override;

    void flush() override;

private:
    MultilayerGroundTexturePrivate *_internal;

    std::mt19937_64 _rng;


    void process(Terrain &terrain, Image &image, vec2i tileCoords,
                 int parentGap);
};

} // namespace world

#endif