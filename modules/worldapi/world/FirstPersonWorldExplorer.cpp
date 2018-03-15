#include "FirstPersonWorldExplorer.h"

#include "../maths/MathsHelper.h"

using namespace maths;

FirstPersonWorldExplorer::FirstPersonWorldExplorer(double minAngle, double punctumProximum)
        : _origin({0, 0, 0}), _angularResolution(minAngle), _punctumProximum(punctumProximum), _maxDistance(20000) {

}

void FirstPersonWorldExplorer::setOrigin(const maths::vec3d &origin) {
    _origin = origin;
}

void FirstPersonWorldExplorer::setMinAngle(double minAngle) {
    _angularResolution = minAngle;
}

void FirstPersonWorldExplorer::setPunctumProximum(double punctumProximum) {
    _punctumProximum = punctumProximum;
}

maths::vec3d FirstPersonWorldExplorer::getChunkNearestPoint(const WorldZone &zone) {
    return zone->getAbsoluteOffset() + zone->getChunk().getSize() / 2;
}

double FirstPersonWorldExplorer::getDetailSizeAt(const vec3d &pos) {
    double length = max(_punctumProximum, _origin.length(pos));
    return _angularResolution * length;
}
