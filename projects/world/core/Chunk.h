#pragma once

#include "world/core/WorldConfig.h"

#include <tuple>
#include <memory>
#include <iterator>
#include <functional>

#include "world/math/BoundingBox.h"
#include "world/math/MathsHelper.h"

#include "ICollector.h"
#include "WorldNode.h"

namespace world {

/**  */
class WORLDAPI_EXPORT Chunk : public WorldNode {
public:
    Chunk(const vec3d &size);

    Chunk(const Chunk &chunk) = delete;

    void setResolutionLimits(double min, double max);

    double getMinResolution() const { return _minResolution; }

    double getMaxResolution() const { return _maxResolution; }

    const vec3d &getSize() const { return _size; }

private:
    double _minResolution = 0;
    double _maxResolution = 1e100;
    vec3d _size;
};
} // namespace world
