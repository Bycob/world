#pragma once

#include "core/WorldConfig.h"

#include "core/WeightedSkeletton.h"

namespace world {

class WORLDAPI_EXPORT TreeInfo {
public:
    int _level = 0;

    /// Size of the branch terminated by this node.
    double _size = 0;
    /// Rotation around z axis in radians, from 0 to 2 * PI.
    /// 0 means that the branch points toward positive x.
    double _theta = 0;
    /// angle with z axis in radians, from 0 to PI.
    /// 0 means that the branch points toward positive z.
    double _phi = 0;

    /// weight of the branch, ie. thickness.
    double _weight = 1;

    /// First vertex in the main trunk mesh (used when
    /// generating leaves)
    int _firstVert = 0;

    /// Last vertex in the main trunk mesh (used when
    /// generating leaves)
    int _lastVert = 0;
};

using TreeSkeletton = WeightedSkeletton<TreeInfo>;

} // namespace world
