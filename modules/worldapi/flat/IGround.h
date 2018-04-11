#ifndef WORLD_IGROUND_H
#define WORLD_IGROUND_H

#include "core/WorldConfig.h"

#include "core/WorldZone.h"
#include "core/ICollector.h"

namespace world {

    class FlatWorld;
    class FlatWorldCollector;

    class WORLDAPI_EXPORT IGround {
    public:
        virtual double observeAltitudeAt(WorldZone zone, double x, double y) = 0;

		virtual void collectZone(const WorldZone &zone, ICollector &collector) = 0;

		// TODO change arguments order
        virtual void collectZone(const WorldZone &zone, FlatWorld &world, FlatWorldCollector &collector) = 0;
    };
}

#endif //WORLD_IGROUND_H
