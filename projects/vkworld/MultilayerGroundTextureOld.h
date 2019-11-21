#ifndef VKWORLD_MULTILAYER_GROUND_TEXTURE_OLD_H
#define VKWORLD_MULTILAYER_GROUND_TEXTURE_OLD_H

#include "VkWorldConfig.h"

#include <random>

#include <world/terrain/ITerrainWorker.h>

#include "DistributionParams.h"

namespace world {

class MultilayerGroundTextureOldPrivate;

class VKWORLD_EXPORT MultilayerGroundTextureOld : public ITerrainWorker {
public:
    MultilayerGroundTextureOld();

    ~MultilayerGroundTextureOld() override;

    /** Add layers corresponding to a world similar to the Earth. */
    void addDefaultLayers();

    void addLayer(const DistributionParams &distribution,
                  const std::string &textureShader);

    void processTerrain(Terrain &terrain) override;

    void processTile(ITileContext &context) override;

    void flush() override;

private:
    MultilayerGroundTextureOldPrivate *_internal;

    std::mt19937_64 _rng;


    void process(Terrain &terrain, Image &image, vec2i tileCoords,
                 int parentGap);
};

} // namespace world

#endif