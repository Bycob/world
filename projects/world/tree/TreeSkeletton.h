#pragma once

#include "world/core/WorldConfig.h"

#include "world/core/WeightedSkeletton.h"
#include "world/math/Bezier.h"

namespace world {

class WORLDAPI_EXPORT TreeInfo : public SkelettonNodeInfo {
public:
    /// Curve followed by the branch
    BezierCurve _curve;
    /// The size of a branch is generally smaller on the high end of the
    /// branch. This is the area of the branch on its low end.
    double _startArea = 0;
    /// The size of a branch is generally smaller on the high end of the
    /// branch. This is the area of the branch on its high end.
    double _endArea = 0;
    /// Weight of the branch alone (without all its children)
    /// The weight is an approximation of the volume of the tree.
    double _selfWeight = 0;
    /// True if the branch should be connected to the end of its parent
    /// Always false for root (since it has no parent)
    bool _connected = true;
    /// True if this branch has at least one child that is connected to
    /// it. If _hasNext is false, the branch should not be left open when
    /// generating trunk.
    bool _hasNext = true;

    // ==== UTILITY VARIABLES =====

    /// First vertex in the main trunk mesh (used when
    /// generating leaves)
    int _firstVert = -1;
    /// Last vertex in the main trunk mesh (used when
    /// generating leaves)
    int _lastVert = -1;
    /// Parameter corresponding to the starting point on the bezier curve
    /// of parent
    double _t = 1;
    /// Rotation around z axis in radians, from 0 to 2 * PI.
    /// 0 means that the branch points toward positive x.
    double _theta = 0;
    /// angle with z axis in radians, from 0 to PI.
    /// 0 means that the branch points toward positive z.
    double _phi = 0;
    /// Size of the branch terminated by this node.
    double _size = 0;
    /// Weight normalized towards other branch with the same parents
    double _normWeight = 0;

    /// Number of children this branch has
    int _forkCount = 0;
    /// ID to identify a single child in a group of children.
    int _forkId = 0;
};

std::ostream &operator<<(std::ostream &os, TreeInfo &ti);

using TreeSkeletton = WeightedSkeletton<TreeInfo>;
using TreeNode = SkelettonNode<TreeInfo>;

} // namespace world
