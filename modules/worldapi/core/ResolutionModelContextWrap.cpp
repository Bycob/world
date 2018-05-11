#include "ResolutionModelContextWrap.h"

namespace world {

ResolutionModelContextWrap::ResolutionModelContextWrap(
    const IResolutionModel &resolutionModel)
        : _wrapped(resolutionModel) {}

void ResolutionModelContextWrap::setOffset(const vec3d &offset) {
    _offset = offset;
}

double ResolutionModelContextWrap::getResolutionAt(const WorldZone &zone,
                                                   const vec3d &coord) const {
    return _wrapped.getResolutionAt(zone, coord + _offset);
}

double ResolutionModelContextWrap::getResolutionAt(
    const world::vec3d &coord) const {
    return _wrapped.getResolutionAt(coord + _offset);
}

} // namespace world
