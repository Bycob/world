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

/** A chunk is a 3D block of space that can contain other objects.
 * It is generally part of a chunk system that covers all the space
 * with a grid of chunks. */
class WORLDAPI_EXPORT Chunk : public WorldNode {
public:
    Chunk(const vec3d &size = {}, double minRes = 0, double maxRes = 1e100);

    void setResolutionLimits(double min, double max);

    double getMinResolution() const { return _minResolution; }

    double getMaxResolution() const { return _maxResolution; }

    /** Returns true if the chunk contains objects of the given resolution */
    bool supportsResolution(double resolution) const;

    const vec3d &getSize() const { return _size; }

private:
    double _minResolution;
    double _maxResolution;
    vec3d _size;
};
} // namespace world
