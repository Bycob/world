#ifndef VKWORLD_MULTILAYER_GROUND_TEXTURE_H
#define VKWORLD_MULTILAYER_GROUND_TEXTURE_H

#include "VkWorldConfig.h"

#include <random>

#include <world/terrain/ITerrainWorker.h>

#include "DistributionParams.h"

namespace world {

class MultilayerGroundTexturePrivate;

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

    /** Get specific layer texture */
    Image getTexture(int lod, int layerId);

private:
    MultilayerGroundTexturePrivate *_internal;
};

} // namespace world

#endif // VKWORLD_MULTILAYER_GROUND_TEXTURE_H
