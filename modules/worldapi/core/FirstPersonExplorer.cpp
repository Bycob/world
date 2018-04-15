#include "FirstPersonExplorer.h"

#include "math/MathsHelper.h"

namespace world {

FirstPersonExplorer::FirstPersonExplorer(double eyeResolution, double fov,
                                         double punctumProximum)
        : _eyeResolution(eyeResolution), _fov(fov),
          _punctumProximum(punctumProximum), _farDistance(5000),
          _position({0, 0, 0}) {}

void FirstPersonExplorer::setPosition(const vec3d &position) {
    _position = position;
}

void FirstPersonExplorer::setEyeResolution(double resolution) {
    _eyeResolution = resolution;
}

void FirstPersonExplorer::setFOV(double fov) { _fov = fov; }

void FirstPersonExplorer::setPunctumProximum(double punctumProximum) {
    _punctumProximum = punctumProximum;
}

void FirstPersonExplorer::setFarDistance(double maxDistance) {
    _farDistance = maxDistance;
}

vec3d FirstPersonExplorer::getChunkNearestPoint(const WorldZone &zone) {
    vec3d lower = zone->getAbsoluteOffset();
    vec3d upper = lower + zone->getChunk().getSize();

    return {clamp(_position.x, lower.x, upper.x),
            clamp(_position.y, lower.y, upper.y),
            clamp(_position.z, lower.z, upper.z)};
}

double FirstPersonExplorer::getResolutionAt(const vec3d &pos) {
    double length = max(_punctumProximum, _position.length(pos));
    return _eyeResolution / (_fov * M_PI / 180 * length); // _fov * length can
                                                          // be seen as the
                                                          // "image size"
}
} // namespace world
