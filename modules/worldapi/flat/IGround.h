#ifndef WORLD_IGROUND_H
#define WORLD_IGROUND_H

#include "core/WorldConfig.h"

#include "core/WorldZone.h"
#include "core/ICollector.h"
#include "core/IResolutionModel.h"

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
