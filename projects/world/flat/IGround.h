#ifndef WORLD_IGROUND_H
#define WORLD_IGROUND_H

#include "world/core/WorldConfig.h"

#include "world/core/WorldZone.h"
#include "world/core/ICollector.h"
#include "world/core/IResolutionModel.h"

namespace world {

class FlatWorld;

class WORLDAPI_EXPORT IGround {
public:
    virtual ~IGround() = default;

    virtual double observeAltitudeAt(WorldZone zone, double x, double y) = 0;

    virtual void collectZone(const WorldZone &zone, ICollector &collector,
                             const IResolutionModel &resolutionModel) = 0;
};
} // namespace world

#endif // WORLD_IGROUND_H
