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

    auto &info = node->getInfo();
    const auto parentCurve = info._curve;

    // SIDE BRANCHES

    const double sideBudget = info._endWeight - info._weight;
    const u32 sideCount = 5;

    // localization of the starts of the branches on the side
    std::vector<vec3d> sideStarts;
    std::vector<vec3d> sideDirs;

    for (u32 i = 0; i < sideCount; ++i) {
        double t = rand(_rng);
        vec3d pos = parentCurve.getPointAt(t);

        // pick direction
        double theta = rand(_rng) * 2 * M_PI;

        // pick inclination towards main branch

        // pick weight: overall the same

        // drop branch if underweight
    }

    // END BRANCHES

    // Does gravity influence on branches ? rigidity is a parameter

    const double endBudget = info._endWeight;
    const u32 endCount = 8;

    // directions of the branches at the end.
    std::vector<vec3d> endDirs;

    for (u32 i = 0; i < endCount; ++i) {
        // Pick a random direction with uniform spheric distribution
        double phi = rand(_rng);
        phi *= phi;
        double theta = rand(_rng) * 2 * M_PI;

        // Apply parameters to phi
        // ...
        phi *= M_PI_2; // Can be greater than pi / 2?
        const double sinPhi = sin(phi);

        vec3d dir{cos(theta) * sinPhi, sin(theta) * sinPhi, cos(phi)};

        // Weight distribution: few big branches, few very small branches, most
        // are average

        // drop branch if underweight
    }

    // set "end" property
    if (endDirs.empty()) {
        info._hasNext = false;
    }
}

void TreeSkelettonWorker::initBranch(SkelettonNode<TreeInfo> *node) {}

void TreeSkelettonWorker::balanceSides(std::vector<vec3d> &pos,
                                       std::vector<vec3d> &dirs,
                                       const std::vector<double> &weights) {
    std::vector<vec3d> gradients;

    for (u32 i = 0; i < _itcount; ++i) {
        for (size_t j = 0; j < dirs.size(); ++j) {
            for (size_t k = j + 1; k < dirs.size(); ++k) {
                // Compute gradient j -> k & k <- j
            }

            // Apply gradient
        }
    }
}

void TreeSkelettonWorker::balanceSphere(std::vector<vec3d> &dirs,
                                        const std::vector<double> &weights) {
    std::vector<vec3d> gradients;

    for (u32 i = 0; i < _itcount; ++i) {
        for (size_t j = 0; j < dirs.size(); ++j) {
            for (size_t k = j + 1; k < dirs.size(); ++k) {
                // Compute gradient j -> k & k <- j
            }

            // Apply gradient
        }
    }
}

} // namespace world
