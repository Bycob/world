#ifndef WORLD_MULTILAYERGROUNDTEXTURE_H
#define WORLD_MULTILAYERGROUNDTEXTURE_H

#include "world/core/WorldConfig.h"

#include "GridStorage.h"
#include "ITerrainWorker.h"
#include "DistributionParams.h"

namespace world {

class WORLDAPI_EXPORT ITextureProvider {
public:
    virtual ~ITextureProvider() = default;

    virtual Image &getTexture(int layer, int lod) = 0;
};

class WORLDAPI_EXPORT MultilayerGroundTexture : public ITerrainWorker {
public:
    struct Element : public IGridElement {
        std::vector<Terrain> _distributions;
    };

    MultilayerGroundTexture(ITextureProvider *texProvider);

    void processTerrain(Terrain &terrain) override;

    void processTile(ITileContext &context) override;

    void addLayer(DistributionParams params);

private:
    GridStorage<Element> _storage;

    ITextureProvider *_texProvider;
    std::vector<DistributionParams> _layers;


    void process(Terrain &terrain, Image &image, const TileCoordinates &tc);
};

} // namespace world

#endif // WORLD_MULTILAYERGROUNDTEXTURE_H
