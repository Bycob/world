#ifndef WORLD_ITERRAINGENERATOR_H
#define WORLD_ITERRAINGENERATOR_H

#include "world/core/WorldConfig.h"

#include "world/core/TileSystem.h"

#include "Terrain.h"
#include "world/core/GridStorage.h"
#include "world/core/NodeCache.h"
#include "world/core/WorldFile.h"
#include "world/core/ICollector.h"

namespace world {

class WORLDAPI_EXPORT TerrainTile {
public:
    TileCoordinates _key;
    Terrain _terrain;
    Mesh _mesh;


    TerrainTile(TileCoordinates key, int size) : _key(key), _terrain(size) {}

    Terrain &terrain() { return _terrain; }

    Image &texture() { return _terrain.getTexture(); }

    Mesh &mesh() { return _mesh; }
};

class WORLDAPI_EXPORT ITileContext {
public:
    virtual ~ITileContext() = default;

    virtual TerrainTile &getTile() const = 0;

    virtual TileCoordinates getCoords() const = 0;

    virtual TileCoordinates getParentCoords() const = 0;

    virtual const ExplorationContext &getExplorationContext() const = 0;
};

class WORLDAPI_EXPORT ITerrainWorker : public ISerializable {
public:
    virtual ~ITerrainWorker() = default;

    /** Get storage associated with this worker. The HeightmapGround may
     * execute some operations on the storage, like tile dropping.
     *
     * This method can return null, if the worker has no storage. */
    virtual GridStorageBase *getStorage() { return nullptr; };

    /** Get cache. If this method returns a cache, then the HeightmapGround
     * will set the parent of this cache to its cache.*/
    virtual NodeCache *getCache() { return nullptr; }

    virtual void processTerrain(Terrain &terrain) = 0;

    virtual void processTile(ITileContext &context) = 0;

    /** If the worker generates additional resources that should be collected
     * with the terrain, they can be with this method.
     * It is assumed that processTile has been called before. */
    virtual void collectTile(ICollector &collector, ITileContext &context){};

    /** This method is called by the HeightmapGround when a tile is read
     * from the cache. It can be useful to recreate some broken links
     * (for example on the terrain material). */
    virtual void onReadingCached(ITileContext &context){};

    /** This method apply all modifications to the terrains before the next
     * worker starts processing. This may be useful if this ITerrainWorker can
     * run several jobs concurrently. */
    virtual void flush(){};
};
} // namespace world

#endif // WORLD_ITERRAINGENERATOR_H
