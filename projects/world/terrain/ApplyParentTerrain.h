#ifndef WORLD_APPLYPARENTTERRAIN_H
#define WORLD_APPLYPARENTTERRAIN_H

#include "world/core/WorldConfig.h"

#include "ITerrainWorker.h"

namespace world {

class WORLDAPI_EXPORT ApplyParentTerrain : public ITerrainWorker {
public:
    ApplyParentTerrain();

    void processTerrain(Terrain &terrain) override;

    void processTile(ITileContext &context) override;

private:
    TerrainGrid _storage;

    double _childRate;
    double _parentOverflow;

    double getContribution(int parentCount, double ratio);
};
} // namespace world

#endif // WORLD_APPLYPARENTTERRAIN_H
