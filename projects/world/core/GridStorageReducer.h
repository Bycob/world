#ifndef WORLD_GRID_STORAGE_REDUCER_H
#define WORLD_GRID_STORAGE_REDUCER_H

#include "world/core/WorldConfig.h"

#include "TileSystem.h"
#include "GridStorage.h"

namespace world {

class WORLDAPI_EXPORT GridStorageReducer {
public:
    GridStorageReducer(TileSystem &tileSystem, u32 maxInstances = 2000)
            : _tileSystem(tileSystem), _maxInstances(maxInstances) {}

    void registerStorage(GridStorageBase *storage);

    void registerAccess(const TileCoordinates &tc);

    /** Reduce storage by deleting tiles that have not beed accessed for a very
     * long time. */
    void reduceStorage();

private:
    TileSystem &_tileSystem;

    u32 _maxInstances;

    u64 _accessCounter = 0;
    std::map<TileCoordinates, u64> _accessTracker;

    std::list<GridStorageBase *> _storages;
};

} // namespace world

#endif // WORLD_GRID_STORAGE_REDUCER_H
