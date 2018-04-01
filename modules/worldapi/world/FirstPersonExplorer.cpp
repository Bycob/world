#include "FirstPersonExplorer.h"

#include "../maths/MathsHelper.h"

namespace world {

    FirstPersonExplorer::FirstPersonExplorer(double minAngle, double punctumProximum)
            : _origin({0, 0, 0}), _angularResolution(minAngle), _punctumProximum(punctumProximum), _maxDistance(20000) {

    }

    void FirstPersonExplorer::setOrigin(const vec3d &origin) {
        _origin = origin;
    }

    void FirstPersonExplorer::setMinAngle(double minAngle) {
        _angularResolution = minAngle;
    }

    void FirstPersonExplorer::setPunctumProximum(double punctumProximum) {
        _punctumProximum = punctumProximum;
    }

    void FirstPersonExplorer::setMaxDistance(double maxDistance) {
        _maxDistance = maxDistance;
    }

    vec3d FirstPersonExplorer::getChunkNearestPoint(const WorldZone &zone) {
        return zone->getAbsoluteOffset() + zone->getChunk().getSize() / 2;
    }

    double FirstPersonExplorer::getDetailSizeAt(const vec3d &pos) {
        double length = max(_punctumProximum, _origin.length(pos));
        return _angularResolution * length;
    }
}
