#pragma once

#include "world/core/WorldConfig.h"

#include "world/core/WeightedSkeletton.h"
#include "world/math/Bezier.h"

namespace world {

class WORLDAPI_EXPORT TreeInfo : public SkelettonNodeInfo {
public:
    /// Curve followed by the branch
    BezierCurve _curve;
    /// The weight of a branch is generally lower on the high end of the
    /// branch. This is the weight of the branch on its high end.
    double _endWeight;
    /// True if the branch should be connected to the end of its parent
    /// Always false for root (since it has no parent)
    bool _connected = true;
    /// True if this branch has at least one child that is connected to
    /// it. If _hasNext is false, the branch should not be left open when
    /// generating trunk.
    bool _hasNext = true;

    /// First vertex in the main trunk mesh (used when
    /// generating leaves)
    int _firstVert = -1;
    /// Last vertex in the main trunk mesh (used when
    /// generating leaves)
    int _lastVert = -1;

    // ==== LEGACY =====

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
};

using TreeSkeletton = WeightedSkeletton<TreeInfo>;
using TreeNode = SkelettonNode<TreeInfo>;

} // namespace world
