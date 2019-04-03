#include "FirstPersonView.h"

#include "world/math/MathsHelper.h"

namespace world {

FirstPersonView::FirstPersonView(double eyeResolution, double fov,
                                         double punctumProximum)
        : _eyeResolution(eyeResolution), _fov(fov),
          _punctumProximum(punctumProximum), _farDistance(5000),
          _position({0, 0, 0}) {}

void FirstPersonView::setPosition(const vec3d &position) {
    _position = position;
}

void FirstPersonView::setEyeResolution(double resolution) {
    _eyeResolution = resolution;
}

void FirstPersonView::setFOV(double fov) { _fov = fov; }

void FirstPersonView::setPunctumProximum(double punctumProximum) {
    _punctumProximum = punctumProximum;
}

void FirstPersonView::setFarDistance(double maxDistance) {
    _farDistance = maxDistance;
}

vec3d FirstPersonView::getNearestPointIn(const BoundingBox &bbox) const {
    vec3d lower = bbox.getLowerBound();
    vec3d upper = bbox.getUpperBound();

    return {clamp(_position.x, lower.x, upper.x),
            clamp(_position.y, lower.y, upper.y),
            clamp(_position.z, lower.z, upper.z)};
}

double FirstPersonView::getResolutionAt(const vec3d &pos) const {
    double length = max(_punctumProximum, _position.length(pos));
    // _fov * length can be seen as the "image size"
    return length <= _farDistance ? _eyeResolution / (_fov * M_PI / 180 * length) : 0.;
}

double FirstPersonView::getMaxResolutionIn(const BoundingBox &bbox) const {
    return getResolutionAt(getNearestPointIn(bbox));
}

BoundingBox FirstPersonView::getBounds() const {
    vec3d far{_farDistance, _farDistance, _farDistance};
    return {_position - far, _position + far};
}

} // namespace world
