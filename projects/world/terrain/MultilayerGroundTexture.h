#ifndef WORLD_MULTILAYERGROUNDTEXTURE_H
#define WORLD_MULTILAYERGROUNDTEXTURE_H

#include "world/core/WorldConfig.h"

#include "ITerrainWorker.h"
#include "DistributionParams.h"
#include "world/core/NodeCache.h"
#include "world/core/GridStorage.h"
#include "world/core/ICollector.h"
#include "world/math/Perlin.h"

namespace world {

// TODO this is an "interface" class and it should not hold cache logic (put it
// in a separate class)
class WORLDAPI_EXPORT ITextureProvider : public ISerializable {
public:
    virtual ~ITextureProvider() = default;

    virtual Image &getTexture(int layer, int lod) = 0;

    void configureCacheOverride(const std::string &path) {
        _cache.setRoot(path);
    }

    void setBasePixelSize(float pixSize) { _basePixelSize = pixSize; }

    bool configureCache(NodeCache &parent, const std::string &id) {
        if (_cache.isAvailable())
            return false;
        else {
            _cache.setChild(parent, id);
            return true;
        }
    }

    void write(WorldFile &wf) const override {
        if (_cache.isRoot()) {
            wf.addString("path", _cache.getDirectory());
        }
    }

    void read(const WorldFile &wf) override {
        std::string path;

        if (wf.readStringOpt("path", path)) {
            configureCacheOverride(path);
        }
    }

protected:
    NodeCache _cache;

    /// World size (in meters) of one pixel at lod 0
    float _basePixelSize = 1;


    std::string getImageId(int layer, int lod) const {
        return "tex" + std::to_string(layer) + "_" + std::to_string(lod);
    }
};

class WORLDAPI_EXPORT MultilayerGroundTexture : public ITerrainWorker {
    WORLD_WRITE_SUBCLASS_METHOD
public:
    struct Element : public IGridElement {
        std::vector<Terrain> _distributions;
        /// One terrain can have only few textures
        std::vector<int> _layerIds;

        void saveTo(NodeCache &cache) const override;

        bool tryLoadFrom(const NodeCache &cache) override;
    };

    MultilayerGroundTexture();

    template <typename T, typename... Args>
    T &setTextureProvider(Args &&... args);

    ITextureProvider &getTextureProvider() const;

    u32 getDistributionResolution() const { return _distribResolution; }

    // TODO deprecate this or hide it
    Element &getTile(const TileCoordinates &tc);

    void processTerrain(Terrain &terrain) override;

    void processTile(ITileContext &context) override;

    void collectTile(ICollector &collector, ITileContext &context) override;

    void addLayer(DistributionParams params);

    GridStorageBase *getStorage() override;

    NodeCache *getCache() override { return &_cache; }

    void write(WorldFile &wf) const override;

    void read(const WorldFile &wf) override;

private:
    GridStorage<Element> _storage;
    NodeCache _cache;

    std::unique_ptr<ITextureProvider> _texProvider;
    std::vector<DistributionParams> _layers;

    Perlin _perlin;

    u32 _distribResolution = 33;
    /// If this flag is false, only the distribution of each texture is
    /// generated.
    bool _generateTexture = true;


    void process(Terrain &terrain, Image &image, const TileCoordinates &tc,
                 const ExplorationContext &ctx);

    void collectTextures(ICollectorChannel<Image> &texChannel,
                         const TileCoordinates &tc,
                         const ExplorationContext &ctx);

    ItemKey getDistributionKey(const TileCoordinates &tc, int id) const;

    ItemKey getTextureKey(int layer, int lod) const;
};

template <typename T, typename... Args>
T &MultilayerGroundTexture::setTextureProvider(Args &&... args) {
    _texProvider = std::make_unique<T>(args...);
    _texProvider->configureCache(_cache, "texProvider");
    return static_cast<T &>(*_texProvider);
}

} // namespace world

#endif // WORLD_MULTILAYERGROUNDTEXTURE_H
