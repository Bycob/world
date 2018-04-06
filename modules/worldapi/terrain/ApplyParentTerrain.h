#ifndef WORLD_APPLYPARENTTERRAIN_H
#define WORLD_APPLYPARENTTERRAIN_H

#include "core/WorldConfig.h"

#include "ITerrainWorker.h"

namespace world {

    class WORLDAPI_EXPORT ApplyParentTerrain : public ITerrainWorker {
    public:
        ApplyParentTerrain();

        void process(Terrain& terrain) override;

        void process(Terrain& terrain, ITerrainWorkerContext &context) override;
    private:
        double _childRate;
        double _parentOverflow;

        double getContribution(int parentCount, double ratio);
    };
}

#endif //WORLD_APPLYPARENTTERRAIN_H
