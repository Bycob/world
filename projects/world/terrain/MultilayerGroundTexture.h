#ifndef WORLD_MULTILAYERGROUNDTEXTURE_H
#define WORLD_MULTILAYERGROUNDTEXTURE_H

#include "world/core/WorldConfig.h"

#include "world/core/GridStorage.h"
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

    MultilayerGroundTexture();

    template <typename T, typename... Args>
    T &setTextureProvider(Args &&... args);

    void processTerrain(Terrain &terrain) override;

    void processTile(ITileContext &context) override;

    void addLayer(DistributionParams params);

    GridStorageBase *getStorage() override;

private:
    GridStorage<Element> _storage;

    std::unique_ptr<ITextureProvider> _texProvider;
    std::vector<DistributionParams> _layers;


    void process(Terrain &terrain, Image &image, const TileCoordinates &tc);
};

template <typename T, typename... Args>
T &MultilayerGroundTexture::setTextureProvider(Args &&... args) {
    _texProvider = std::make_unique<T>(args...);
    return static_cast<T &>(*_texProvider);
}

} // namespace world

#endif // WORLD_MULTILAYERGROUNDTEXTURE_H
