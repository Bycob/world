#ifndef WORLD_FIRSTPERSONWORLDEXPLORER_H
#define WORLD_FIRSTPERSONWORLDEXPLORER_H

#include <worldapi/worldapidef.h>

#include "World.h"
#include "Collector.h"

namespace world {

    class WORLDAPI_EXPORT FirstPersonExplorer {
    public:
        FirstPersonExplorer(double minAngle = 0.002, double punctumProximum = 1);

        void setOrigin(const vec3d &origin);

        void setMinAngle(double minAngle);

        void setPunctumProximum(double punctumProximum);

        template<typename T>
        void explore(T &world, ICollector<T> &collector);

        template<typename T>
        void exploreVertical(T &world, const WorldZone &zone, ICollector<T> &collector);

        vec3d getChunkNearestPoint(const WorldZone &zone);

        double getDetailSizeAt(const vec3d &pos);

    private:
        double _angularResolution;
        double _punctumProximum;
        /** The maximum distance the explorer can see over. In real world,
         * we cannot see farther than a certain distance due to mist, for
         * example. */
        double _maxDistance;
        vec3d _origin;
    };
}

#include "FirstPersonExplorer.inl"

#endif //WORLD_FIRSTPERSONWORLDEXPLORER_H
