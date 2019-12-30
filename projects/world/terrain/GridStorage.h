#ifndef WORLD_GRID_STORAGE_H
#define WORLD_GRID_STORAGE_H

#include "world/core/WorldConfig.h"

#include <type_traits>
#include <map>

#include "world/core/TileSystem.h"
#include "Terrain.h"

namespace world {

class WORLDAPI_EXPORT GridStorageBase {
public:
    virtual ~GridStorageBase() = default;

    virtual bool has(const TileCoordinates &coords) const = 0;
};

class WORLDAPI_EXPORT IGridElement {
public:
    virtual ~IGridElement() = default;

    // Add save(...) and load(...) when serialization will be implemented
};

class WORLDAPI_EXPORT TerrainElement : public IGridElement {
public:
    Terrain _terrain;

    TerrainElement(int size) : _terrain(size) {}

    TerrainElement(const Terrain &terrain) : _terrain(terrain) {}
};

template <
    typename TElement,
    std::enable_if_t<std::is_base_of<IGridElement, TElement>::value, int> = 0>
class GridStorage : public GridStorageBase {
public:
    GridStorage() = default;
    ~GridStorage() override = default;

    GridStorage(const GridStorage &other) = delete;
    GridStorage(GridStorage &&other) = default;

    GridStorage &operator=(const GridStorage& other) = delete;
    GridStorage &operator=(GridStorage&& other) = default;

    template <typename... Args>
    TElement &set(const TileCoordinates &coords, Args &&... args) {
        return *(_storage[coords] = std::make_unique<TElement>(args...));
    }

    template <typename... Args>
    TElement &getOrCreate(const TileCoordinates &coords, Args &&... args) {
        auto it = _storage.insert({coords, nullptr});
        if (it.second) {
            it.first->second = std::make_unique<TElement>(args...);
        }
        return *it.first->second;
    }

    template <typename... Args>
    TElement &getOrCreateCallback(const TileCoordinates &coords,
                                  std::function<void(TElement &)> callback,
                                  Args &&... args) {
        auto it = _storage.insert({coords, nullptr});
        if (it.second) {
            it.first->second = std::make_unique<TElement>(args...);
            callback(*it.first->second);
        }
        return *it.first->second;
    }

    bool tryGet(const TileCoordinates &coords, TElement **elemPtr) const {
        auto it = _storage.find(coords);
        if (it != _storage.end()) {
            *elemPtr = it->second.get();
            return true;
        } else {
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

private:
    std::map<TileCoordinates, std::unique_ptr<TElement>> _storage;
};

typedef GridStorage<TerrainElement> TerrainGrid;

} // namespace world

#endif // WORLD_GRID_STORAGE_H
