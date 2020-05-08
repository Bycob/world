#include "BoundingBox.h"

#include "MathsHelper.h"

namespace world {

BoundingBox::BoundingBox() : _lower{0, 0, 0}, _upper{0, 0, 0} {}

BoundingBox::BoundingBox(const vec3d &b1, const vec3d &b2)
        : _lower(min(b1.x, b2.x), min(b1.y, b2.y), min(b1.z, b2.z)),
          _upper(max(b1.x, b2.x), max(b1.y, b2.y), max(b1.z, b2.z)) {}

void BoundingBox::reset(const vec3d &b1, const vec3d &b2) {
    _lower.x = min(b1.x, b2.x);
    _lower.y = min(b1.y, b2.y);
    _lower.z = min(b1.z, b2.z);
    _upper.x = max(b1.x, b2.x);
    _upper.y = max(b1.y, b2.y);
    _upper.z = max(b1.z, b2.z);
}

vec3d BoundingBox::getLowerBound() const { return _lower; }

vec3d BoundingBox::getUpperBound() const { return _upper; }

vec3d BoundingBox::getDimensions() const { return _upper - _lower; }

bool BoundingBox::contains(const vec3d &c) const {
    return _lower.x <= c.x && _lower.y <= c.y && _lower.z <= c.z &&
           _upper.x >= c.x && _upper.y >= c.y && _upper.z >= c.z;
}

void BoundingBox::translate(const vec3d &vec) {
    _lower += vec;
    _upper += vec;
}

void BoundingBox::addPoint(const vec3d &vec) {
    _lower.x = min(_lower.x, vec.x);
    _lower.y = min(_lower.y, vec.y);
    _lower.z = min(_lower.z, vec.z);
    _upper.x = max(_upper.x, vec.x);
    _upper.y = max(_upper.y, vec.y);
    _upper.z = max(_upper.z, vec.z);
}
} // namespace world