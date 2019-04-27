#pragma once

#include "world/core/WorldConfig.h"

#include "world/core/WeightedSkeletton.h"

namespace world {

class WORLDAPI_EXPORT TreeInfo : public SkelettonNodeInfo {
public:
    /// Size of the branch terminated by this node.
    double _size = 0;
    /// Rotation around z axis in radians, from 0 to 2 * PI.
    /// 0 means that the branch points toward positive x.
    double _theta = 0;
    /// angle with z axis in radians, from 0 to PI.
    /// 0 means that the branch points toward positive z.
    double _phi = 0;

    /// Number of children this branch has
    int _forkCount = 0;
    /// ID to identify a single child in a group of children.
    int _forkId = 0;

    /// First vertex in the main trunk mesh (used when
    /// generating leaves)
    int _firstVert = 0;
    /// Last vertex in the main trunk mesh (used when
    /// generating leaves)
    int _lastVert = 0;
};

using TreeSkeletton = WeightedSkeletton<TreeInfo>;
using TreeNode = SkelettonNode<TreeInfo>;

} // namespace world
