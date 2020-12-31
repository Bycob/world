#ifndef WORLD_GRID_STORAGE_H
#define WORLD_GRID_STORAGE_H

#include "world/core/WorldConfig.h"

#include <type_traits>
#include <map>

#include "world/core/TileSystem.h"
#include "NodeCache.h"
#include "world/terrain/Terrain.h"
#include "GridStorageReducer.h"

namespace world {

class WORLDAPI_EXPORT GridStorageBase {
public:
    virtual ~GridStorageBase() = default;

    virtual bool has(const TileCoordinates &coords) const = 0;

    /** Removes the tile from the GridStorageBase. If `keepCache` is set to
     * true, all the cached tile information are kept, if they exist. */
    virtual void remove(const TileCoordinates &coords, bool keepCache) = 0;

    virtual NodeCache &getCache() = 0;

    virtual void setReducer(GridStorageReducer *reducer);

protected:
    GridStorageReducer *_reducer = nullptr;
};

class WORLDAPI_EXPORT IGridElement {
public:
    virtual ~IGridElement() = default;

    virtual void saveTo(NodeCache &cache) const {}

    /** Read cached information about this element. Returns false
     * if either the element does not exist in cache or if its related data
     * are corrupted. */
    // It is required that we create a minimum, ungenerated version of the
    // tile before reading the cache
    virtual bool tryLoadFrom(const NodeCache &cache) {}
};

class WORLDAPI_EXPORT TerrainElement : public IGridElement {
public:
    Terrain _terrain;

    TerrainElement(int size) : _terrain(size) {}

    TerrainElement(Terrain terrain) : _terrain(std::move(terrain)) {}

    TerrainElement(Terrain &&terrain) : _terrain(terrain) {}

    void saveTo(NodeCache &cache) const override {
        cache.saveTerrain("terrain", _terrain);
    }

    bool tryLoadFrom(const NodeCache &cache) {
        return cache.readTerrainInplace("terrain", _terrain);
    }
};

template <
    typename TElement,
    std::enable_if_t<std::is_base_of<IGridElement, TElement>::value, int> = 0>
class GridStorage : public GridStorageBase {
public:
    NodeCache _cache;


    GridStorage() = default;
    ~GridStorage() override {
        // Cache every remaining tile in storage
        cacheAll();
    }

    GridStorage(const GridStorage &other) = delete;
    GridStorage(GridStorage &&other) noexcept = default;

    GridStorage &operator=(const GridStorage &other) = delete;
    GridStorage &operator=(GridStorage &&other) noexcept = default;

    template <typename... Args>
    TElement &set(const TileCoordinates &coords, Args &&... args) {
        if (_reducer != nullptr)
            _reducer->registerAccess(coords);
        return *(_storage[coords] = std::make_unique<TElement>(args...));
    }

    /** Get the element at the given coordinates. If this element does
     * not exist, it is created with the arguments passed in parameters.
     * If the element is cached, it is retrieved from the cache. */
    template <typename... Args>
    TElement &getOrCreate(const TileCoordinates &coords, Args &&... args) {
        if (_reducer != nullptr)
            _reducer->registerAccess(coords);
        auto it = _storage.insert({coords, nullptr});
        if (it.second) {
            it.first->second = std::make_unique<TElement>(args...);
            readTileFromCache(coords, *it.first->second);
        }
        return *it.first->second;
    }

    /** Get the element at the given coordinates. If this element does
     * not exist, it is created with the arguments passed in parameters,
     * and the callback is called on this element.
     * If the element was cached, it is retrieved from the cache and the
     * callback is not called. */
    template <typename... Args>
    TElement &getOrCreateCallback(const TileCoordinates &coords,
                                  std::function<void(TElement &)> callback,
                                  Args &&... args) {
        if (_reducer != nullptr)
            _reducer->registerAccess(coords);
        auto it = _storage.insert({coords, nullptr});
        if (it.second) {
            it.first->second = std::make_unique<TElement>(args...);

            if (!readTileFromCache(coords, *it.first->second)) {
                callback(*it.first->second);
            }
        }
        return *it.first->second;
    }

    bool tryGet(const TileCoordinates &coords, TElement **elemPtr) const {
        auto it = _storage.find(coords);
        if (it != _storage.end()) {
            if (_reducer != nullptr)
                _reducer->registerAccess(coords);
            *elemPtr = it->second.get();
            return true;
        } else {
            // TODO make a "tryGetFromCache" function that gives minimal set of
            // parameters to initialize the grid element
            // since not all parameters are in the cache, only data
            return false;
        }
    }

    TElement &get(const TileCoordinates &coords) const {
        TElement *elemPtr;
        if (tryGet(coords, &elemPtr)) {
            return *elemPtr;
        } else {
            std::stringstream sstream;
            sstream << "Tile not found: " << coords._pos << ", " << coords._lod;
            throw std::runtime_error(sstream.str());
        }
    }

    optional<TElement &> getopt(const TileCoordinates &coords) const {
        TElement *elemPtr;
        if (tryGet(coords, &elemPtr)) {
            return *elemPtr;
        } else {
            return nullopt;
        }
    }

    bool has(const TileCoordinates &coords) const override {
        return _storage.find(coords) != _storage.end();
    }

    void remove(const TileCoordinates &coords, bool keepCache) override {
        auto it = _storage.find(coords);

        if (it == _storage.end()) {
            return;
        }

        if (keepCache) {
            writeTileToCache(it->first, *it->second);
        } else {
            NodeCache tileCache(_cache, coords.toKey());
            tileCache.clear();
        }

        _storage.erase(it);
    }

    size_t size() const { return _storage.size(); }

    /** Save all the tiles to the cache. */
    void cacheAll() {
        if (_cache.isAvailable()) {
            for (auto &e : _storage) {
                writeTileToCache(e.first, *e.second);
            }
        }
    }

    NodeCache &getCache() override { return _cache; }

private:
    std::map<TileCoordinates, std::unique_ptr<TElement>> _storage;


    void writeTileToCache(const TileCoordinates &coords, TElement &elem) {
        if (_cache.isAvailable()) {
            NodeCache tileCache(_cache, coords.toKey());
            dynamic_cast<IGridElement *>(&elem)->saveTo(tileCache);
        }
    }

    /** Try to get cached tile. Set the pointer and returns true if
     * success, return false otherwise. */
    bool readTileFromCache(const TileCoordinates &coords, TElement &elem) {
        if (!_cache.isAvailable()) {
            return false;
        } else {
            NodeCache tileCache(_cache, coords.toKey());
            return elem.tryLoadFrom(tileCache);
        }
    }
};

typedef GridStorage<TerrainElement> TerrainGrid;

} // namespace world

#endif // WORLD_GRID_STORAGE_H
