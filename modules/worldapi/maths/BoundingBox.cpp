#include "BoundingBox.h"

namespace world {

    BoundingBox::BoundingBox(const vec3d &b1, const vec3d &b2)
            : _lower(min(b1.x, b2.x), min(b1.y, b2.y), min(b1.z, b2.z)),
              _upper(max(b1.x, b2.x), max(b1.y, b2.y), max(b1.z, b2.z)) {

    }

    void BoundingBox::reset(const vec3d &b1, const vec3d &b2) {
        _lower.x = min(b1.x, b2.x);
        _lower.y = min(b1.y, b2.y);
        _lower.z = min(b1.z, b2.z);
        _upper.x = max(b1.x, b2.x);
        _upper.y = max(b1.y, b2.y);
        _upper.z = max(b1.z, b2.z);
    }

    vec3d BoundingBox::getLowerBound() const {
        return _lower;
    }

    vec3d BoundingBox::getUpperBound() const {
        return _upper;
    }
}