#ifndef WORLD_MULTILAYERGROUNDTEXTURE_H
#define WORLD_MULTILAYERGROUNDTEXTURE_H

#include "world/core/WorldConfig.h"

#include "world/core/GridStorage.h"
#include "ITerrainWorker.h"
#include "DistributionParams.h"
#include "world/core/NodeCache.h"
#include "world/math/Perlin.h"

namespace world {

class WORLDAPI_EXPORT ITextureProvider {
public:
    virtual ~ITextureProvider() = default;

    virtual Image &getTexture(int layer, int lod) = 0;

    void configureCacheOverride(const std::string &path) {
        _cache.setRoot(path);
    }

    bool configureCache(NodeCache &parent, const std::string &id) {
        if (_cache.isAvailable())
            return false;
        else {
            _cache.setChild(parent, id);
            return true;
        }
    }

protected:
    NodeCache _cache;

    std::string getImageId(int layer, int lod) const {
        return "tex" + std::to_string(layer) + "_" + std::to_string(lod);
    }
};

class WORLDAPI_EXPORT MultilayerGroundTexture : public ITerrainWorker {
    WORLD_WRITE_SUBCLASS_METHOD
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

    void write(WorldFile &wf) const;

    void read(const WorldFile &wf);

private:
    GridStorage<Element> _storage;

    std::unique_ptr<ITextureProvider> _texProvider;
    std::vector<DistributionParams> _layers;

    Perlin _perlin;


    void process(Terrain &terrain, Image &image, const TileCoordinates &tc);
};

template <typename T, typename... Args>
T &MultilayerGroundTexture::setTextureProvider(Args &&... args) {
    _texProvider = std::make_unique<T>(args...);
    return static_cast<T &>(*_texProvider);
}

} // namespace world

#endif // WORLD_MULTILAYERGROUNDTEXTURE_H
