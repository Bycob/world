#ifndef VKWORLD_MULTILAYER_GROUND_TEXTURE_H
#define VKWORLD_MULTILAYER_GROUND_TEXTURE_H

#include "VkWorldConfig.h"

#include <random>

#include <world/terrain/ITerrainWorker.h>
#include <world/terrain/DistributionParams.h>

namespace world {

class MultilayerGroundTexturePrivate;

class VKWORLD_EXPORT VkwMultilayerGroundTexture : public ITerrainWorker {
public:
    VkwMultilayerGroundTexture();

    ~VkwMultilayerGroundTexture() override;

    /** Add layers corresponding to a world similar to the Earth. */
    void addDefaultLayers();

    void addLayer(const DistributionParams &distribution,
                  const std::string &textureShader);

    void processTerrain(Terrain &terrain) override;

    void processTile(ITileContext &context) override;

    void flush() override;

private:
    MultilayerGroundTexturePrivate *_internal;
};

} // namespace world

#endif // VKWORLD_MULTILAYER_GROUND_TEXTURE_H
