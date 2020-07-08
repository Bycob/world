#include "TreeSkelettonWorker.h"

namespace world {
WORLD_REGISTER_CHILD_CLASS(ITreeWorker, TreeSkelettonWorker,
                           "TreeSkelettonWorker")

TreeSkelettonWorker::TreeSkelettonWorker() : _rng(std::random_device()()) {}

void TreeSkelettonWorker::processInstance(TreeInstance &tree,
                                          double resolution) {}

TreeSkelettonWorker *TreeSkelettonWorker::clone() const {
    return new TreeSkelettonWorker();
}

void TreeSkelettonWorker::forkNode(SkelettonNode<TreeInfo> *node) {
    std::uniform_real_distribution<double> rand(0, 1);
    std::exponential_distribution<double> weightDistrib(_weightLambda);

    auto &info = node->getInfo();
    const auto parentCurve = info._curve;

    // SIDE BRANCHES
    std::vector<SkelettonNode<TreeInfo> *> sideBranches;

    // TODO multiply by random factor
    const double sideBudget = info._endWeight - info._weight;
    const u32 sideCount = 5; // TODO randomize

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
    }

    // Weight normalization
    for (auto it{sideBranches.begin()}; it != sideBranches.end(); ++it) {
        auto &childInfo = (*it)->getInfo();
        childInfo._weight /= weightSum;

        // drop branch if underweight
        if (childInfo._weight / sideBudget < _minWeight) {
            sideBranches.erase(it);
            it--;
        }
    }

    balanceSides(sideBranches);

    for (auto &sideBranch : sideBranches) {
        auto &childInfo = sideBranch->getInfo();
        // pick inclination towards main branch
        childInfo._phi = 0.25 * M_PI; // TODO randomize

        initBranch(node, sideBranch);
    }

    // END BRANCHES
    std::vector<SkelettonNode<TreeInfo> *> endBranches;

    // Does gravity influence on branches ? rigidity is a parameter
    // -> Gravity also have an influence on side branches

    const double endBudget = info._endWeight;
    const u32 endCount = 8;
    const vec3d endPos = parentCurve._pts[3];

    for (u32 i = 0; i < endCount; ++i) {
        auto *childNode = node->createChild({});
        endBranches.push_back(childNode);

        auto &childInfo = childNode->getInfo();

        // Pick a random direction with uniform spheric distribution
        childInfo._phi = rand(_rng);
        childInfo._phi *= childInfo._phi;
        childInfo._theta = rand(_rng) * 2 * M_PI;

        // Apply parameters to phi
        // ...
        childInfo._phi *= M_PI_2; // Can be greater than pi / 2?

        // Weight distribution: few big branches, few very small branches, most
        // are average
    }

    balanceSphere(endBranches);

    for (auto &sideBranch : sideBranches) {
        initBranch(node, sideBranch);
    }

    // set "end" property
    if (endBranches.empty()) {
        info._hasNext = false;
    }
}

void TreeSkelettonWorker::initBranch(SkelettonNode<TreeInfo> *parent,
                                     SkelettonNode<TreeInfo> *child) {
    auto &info = parent->getInfo();
    const auto parentCurve = info._curve;
    auto &childInfo = child->getInfo();

    vec3d ez = info._curve.getDerivativeAt(childInfo._t).normalize();
    vec3d ex = vec3d::X();
    vec3d ey = vec3d::Y();

    if (abs(ez.x) + abs(ez.y) >= std::numeric_limits<double>::epsilon() * 2) {
        ey = ez.crossProduct(vec3d::Z()).normalize();
        ex = ey.crossProduct(ez).normalize();
    }

    const double sinPhi = sin(childInfo._phi);
    vec3d local{cos(childInfo._theta) * sinPhi, sin(childInfo._theta) * sinPhi,
                cos(childInfo._phi)};

    vec3d dir = ex * local.x + ey * local.y + ez * local.z;
    // TODO turn into parameters
    const double size = 5;
    const double bendFactor = 0.2;

    vec3d origin = parentCurve.getPointAt(childInfo._t);
    vec3d end = origin + dir * size;
    childInfo._curve = {origin, end, origin + ez * size * bendFactor,
                        origin + dir * size * (1 - bendFactor)};

    // TODO compute end weight
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

                double g = infoj._weight * infok._weight / sqdiff;
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
