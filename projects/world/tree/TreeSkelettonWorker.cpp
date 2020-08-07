#include "TreeSkelettonWorker.h"
#include "Tree.h"

namespace world {
WORLD_REGISTER_CHILD_CLASS(ITreeWorker, TreeSkelettonWorker,
                           "TreeSkelettonWorker")

TreeSkelettonWorker::TreeSkelettonWorker() : _rng(std::random_device()()) {}

void TreeSkelettonWorker::processInstance(TreeInstance &tree,
                                          double resolution) {

    // Create first node
    auto *node = tree._skeletton.getPrimaryNode();
    auto &info = node->getInfo();

    // Setup
    info._t = 0;
    info._theta = 0;
    info._phi = M_PI_2;
    info._hasNext = true;
    info._weight = _startWeight;
    info._selfWeight = _selfWeight * info._weight;
    info._size = 1.5;
    info._startArea = 0.06;
    info._endArea = 2 * info._selfWeight / info._size - info._startArea;
    info._curve = {{0},
                   {0, 0, info._size},
                   {0, 0, info._size / 3},
                   {0, 0, -info._size / 3}};

    forkNode(node);
}

TreeSkelettonWorker *TreeSkelettonWorker::clone() const {
    return new TreeSkelettonWorker();
}

void TreeSkelettonWorker::forkNode(SkelettonNode<TreeInfo> *node) {
    std::uniform_real_distribution<double> rand(0, 1);
    std::exponential_distribution<double> weightDistrib(_weightLambda);

    auto &parentInfo = node->getInfo();
    const double budget = parentInfo._weight - parentInfo._selfWeight;

    // TODO randomize
    const double sideRatio = _sideRatio;
    // TODO randomize
    const u32 endCount = _endSplit;
    // TODO randomize
    const u32 sideCount = _sideSplit;

    // === SIDE BRANCHES
    std::vector<SkelettonNode<TreeInfo> *> sideBranches;

    // TODO reduce by a random amount
    const double sideBudget = budget * sideRatio;

    double weightSum = 0;

    for (u32 i = 0; i < sideCount; ++i) {
        auto *childNode = node->createChild({});
        sideBranches.push_back(childNode);

        auto &childInfo = childNode->getInfo();

        childInfo._t = rand(_rng);

        // pick direction
        childInfo._theta = rand(_rng) * 2 * M_PI;

        // pick weight: overall the same
        childInfo._weight = 1 + weightDistrib(_rng);
        weightSum += childInfo._weight;

        childInfo._connected = false;
        childInfo._forkId = i;
    }

    normalizeWeights(sideBranches, sideBudget, weightSum);
    balanceSides(sideBranches);

    for (auto &sideBranch : sideBranches) {
        auto &childInfo = sideBranch->getInfo();
        // pick inclination towards main branch
        // TODO randomize
        childInfo._phi = 0.25 * M_PI;

        initBranch(node, sideBranch);
    }


    // === END BRANCHES
    std::vector<SkelettonNode<TreeInfo> *> endBranches;

    // Does gravity influence on branches ? rigidity is a parameter
    // -> Gravity also have an influence on side branches

    const double endBudget = budget * (1 - sideRatio);

    weightSum = 0;

    for (u32 i = 0; i < endCount; ++i) {
        auto *childNode = node->createChild({});
        endBranches.push_back(childNode);

        auto &childInfo = childNode->getInfo();

        // Pick a random direction with uniform spheric distribution
        childInfo._theta = rand(_rng) * 2 * M_PI;

        childInfo._phi = rand(_rng);
        childInfo._phi *= childInfo._phi;

        // Apply parameters to phi
        // ...
        // Can be greater than pi / 2?
        childInfo._phi *= M_PI_2;

        // Weight distribution: few big branches, few very small branches, most
        // are average
        // TODO See if the method to fix weights does what we want it to do.
        childInfo._weight = 1 + weightDistrib(_rng);
        weightSum += childInfo._weight;

        childInfo._connected = true;
        childInfo._forkId = i + sideCount;
    }

    normalizeWeights(endBranches, endBudget, weightSum);
    balanceSphere(endBranches);

    double maxArea = 0;

    for (auto &endBranch : endBranches) {
        initBranch(node, endBranch);
        maxArea = max(maxArea, endBranch->getInfo()._startArea);
    }

    if (endBranches.empty()) {
        // set "end" property
        parentInfo._hasNext = false;
    } else {
        // set area according to children
        parentInfo._endArea = maxArea;
    }

    // Fork
    if (_maxFork > parentInfo._forkCount) {
        for (auto &sideBranch : sideBranches) {
            forkNode(sideBranch);
        }

        for (auto &endBranch : endBranches) {
            forkNode(endBranch);
        }
    }
}

void TreeSkelettonWorker::normalizeWeights(
    std::vector<SkelettonNode<TreeInfo> *> &branches, double budget,
    double weightSum) {

    for (auto it{branches.begin()}; it != branches.end(); ++it) {
        auto &childInfo = (*it)->getInfo();
        childInfo._normWeight = childInfo._weight / weightSum;
        childInfo._weight = childInfo._normWeight * budget;

        // drop branch if underweight
        if (childInfo._weight < _minWeight) {
            (*it)->remove();
            branches.erase(it);
            it--;
        }
    }
}

void TreeSkelettonWorker::initBranch(SkelettonNode<TreeInfo> *parent,
                                     SkelettonNode<TreeInfo> *child) {
    auto &parentInfo = parent->getInfo();
    auto &childInfo = child->getInfo();


    childInfo._forkCount = parentInfo._forkCount + 1;

    // Find parameters
    // TODO randomize (how much weight left to further branches)
    childInfo._selfWeight = childInfo._weight * _selfWeight;

    // TODO randomize (shape of the branch)
    const double shapeFactor = _shapeFactor;
    const double size =
        pow(childInfo._selfWeight * shapeFactor * shapeFactor, 1. / 3.);
    childInfo._size = size;

    // TODO randomize (difference between start and end)
    childInfo._startArea = childInfo._selfWeight / size * 1.1;
    childInfo._endArea =
        2 * childInfo._selfWeight / childInfo._size - childInfo._startArea;

    // TODO radomize / take gravity into account
    const double bendFactor = _bendFactor;


    // Compute curve
    const auto parentCurve = parentInfo._curve;
    vec3d ez = parentInfo._curve.getDerivativeAt(childInfo._t).normalize();
    vec3d ex = vec3d::X();
    vec3d ey = vec3d::Y();

    if (abs(ez.x) + abs(ez.y) >= std::numeric_limits<double>::epsilon() * 2) {
        ey = ez.crossProduct(vec3d::Z()).normalize();
        ex = ey.crossProduct(ez).normalize();
    }

    const double sinPhi = sin(childInfo._phi);
    vec3d local{cos(childInfo._theta) * sinPhi, sin(childInfo._theta) * sinPhi,
                cos(childInfo._phi)};

    // Direction of the branch from start to end.
    vec3d dir = ex * local.x + ey * local.y + ez * local.z;

    vec3d origin = parentCurve.getPointAt(childInfo._t);
    // TODO Move end according to gravity
    vec3d end = origin + dir * size;

    vec3d w1 = mix(dir, ez, bendFactor);
    // TODO check dot product formulae
    vec3d w2 = mix(-dir, ez - dir * ez.dotProduct(dir) * 2, bendFactor);

    childInfo._curve = {origin, end, w1 * size * 0.25, w2 * size * 0.25};
    childInfo._position = end;
}

void TreeSkelettonWorker::balanceSides(
    std::vector<SkelettonNode<TreeInfo> *> &branches) {
    std::vector<double> gradients(branches.size(), 0);

    for (u32 i = 0; i < _itcount; ++i) {
        if (i != 0)
            std::fill(gradients.begin(), gradients.end(), 0);

        for (size_t j = 0; j < branches.size(); ++j) {
            TreeInfo &infoj = branches[j]->getInfo();

            for (size_t k = j + 1; k < branches.size(); ++k) {
                TreeInfo &infok = branches[k]->getInfo();

                // Compute gradient j -> k & k <- j
                double sqdiff = wrapAngle(infok._theta - infoj._theta);

                // minimum distance to avoid glitches (e.g. if angles are the
                // same)
                const double min = 0.1;
                if (sqdiff < min) {
                    sqdiff = min;
                }
                sqdiff *= sqdiff;

                // repulsive force
                double g = -infoj._normWeight * infok._normWeight / sqdiff;
                gradients[j] -= g;
                gradients[k] += g;
            }

            // Apply gradient
            infoj._theta += _step * gradients[j];
        }
    }
}

void TreeSkelettonWorker::balanceSphere(
    std::vector<SkelettonNode<TreeInfo> *> &branches) {
    std::vector<vec2d> gradients;

    for (u32 i = 0; i < _itcount; ++i) {
        for (size_t j = 0; j < branches.size(); ++j) {
            TreeInfo &infoj = branches[j]->getInfo();

            for (size_t k = j + 1; k < branches.size(); ++k) {
                TreeInfo &infok = branches[k]->getInfo();
                // Compute gradient j -> k & k <- j
                vec2d diff{infok._theta - infoj._theta,
                           wrapAngle(infok._phi - infoj._phi)};

                // TODO
            }

            // Apply gradient
        }
    }
}

} // namespace world
