#include "ResolutionModelContextWrap.h"

namespace world {

ResolutionModelContextWrap::ResolutionModelContextWrap(
    const IResolutionModel &resolutionModel)
        : _wrapped(resolutionModel) {}

void ResolutionModelContextWrap::setOffset(const vec3d &offset) {
    _offset = offset;
}

double ResolutionModelContextWrap::getResolutionAt(
    const world::vec3d &coord) const {
    return _wrapped.getResolutionAt(coord + _offset);
}

double ResolutionModelContextWrap::getMaxResolutionIn(
    const BoundingBox &bbox) const {
    BoundingBox bbox2 = bbox;
    bbox2.translate(_offset);
    return _wrapped.getMaxResolutionIn(bbox2);
}

BoundingBox ResolutionModelContextWrap::getBounds() const {
    BoundingBox bbox = _wrapped.getBounds();
    bbox.translate(_offset);
    return bbox;
}

} // namespace world
