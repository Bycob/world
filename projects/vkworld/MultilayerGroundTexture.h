#ifndef VKWORLD_MULTILAYER_GROUND_TEXTURE_H
#define VKWORLD_MULTILAYER_GROUND_TEXTURE_H

#include "VkWorldConfig.h"

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
};

class VKWORLD_EXPORT MultilayerGroundTexture : public ITerrainWorker {
public:
    MultilayerGroundTexture();

    ~MultilayerGroundTexture() override;

    void processTerrain(Terrain &terrain) override;

    void processTile(ITileContext &context) override;

    void flush() override;

private:
    MultilayerGroundTexturePrivate *_internal;


    void process(Terrain &terrain, Image &image);
};

} // namespace world

#endif