#ifndef WORLD_IGROUND_H
#define WORLD_IGROUND_H

#include "core/WorldConfig.h"

#include "core/WorldZone.h"

namespace world {

    class FlatWorld;
    class FlatWorldCollector;

    class WORLDAPI_EXPORT IGround {
    public:
        virtual double observeAltitudeAt(WorldZone zone, double x, double y) = 0;

        virtual void collectZone(FlatWorldCollector &collector, FlatWorld &world, const WorldZone &zone) = 0;
    };
}

#endif //WORLD_IGROUND_H
