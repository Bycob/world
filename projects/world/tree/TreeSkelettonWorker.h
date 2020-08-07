
#ifndef WORLD_TREESKELETTONWORKER_H
#define WORLD_TREESKELETTONWORKER_H

#include "world/core/WorldConfig.h"

#include "ITreeWorker.h"
#include "TreeSkeletton.h"

namespace world {

/** Gravity Based Homogeneous Branch Distribution */
class WORLDAPI_EXPORT TreeSkelettonWorker : public ITreeWorker {
    WORLD_WRITE_SUBCLASS_METHOD
public:
    TreeSkelettonWorker();

    void processInstance(TreeInstance &tree, double resolution) override;

    TreeSkelettonWorker *clone() const override;

private:
    std::mt19937 _rng;

    // Tree parameters;
    double _startWeight = 0.5;
    double _selfWeight = 0.15;
    /// A higher lambda means the branches have different weights.
    double _weightLambda = 2;
    double _minWeight = 0.002;
    double _sideRatio = 0.5;
    u32 _sideSplit = 6;
    u32 _endSplit = 3;

    /// The ratio between branch length and branch section.
    double _shapeFactor = 8;
    /// 0 means straight branches, 1 means folded.
    double _bendFactor = 0.5;

    /// Maximum number of nodes from the root
    u32 _maxFork = 2;

    // balancing
    u32 _itcount = 5;
    double _step = 0.01;


    void forkNode(SkelettonNode<TreeInfo> *node);

    /** Normalize branch weights according to the budget. Drop branches
     * that are too thin. */
    void normalizeWeights(std::vector<SkelettonNode<TreeInfo> *> &branches,
                          double budget, double weightSum);

    void initBranch(SkelettonNode<TreeInfo> *parent,
                    SkelettonNode<TreeInfo> *child);

    void balanceSides(std::vector<SkelettonNode<TreeInfo> *> &branches);

    void balanceSphere(std::vector<SkelettonNode<TreeInfo> *> &branches);
};
} // namespace world

#endif // WORLD_TREESKELETTONWORKER_H
