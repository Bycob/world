#ifndef WORLD_TREESKELETTONHOGBASEDWORKER_H
#define WORLD_TREESKELETTONHOGBASEDWORKER_H

#include "world/core/WorldConfig.h"

#include <random>

#include "TreeSkelettonWorker.h"
#include "TreeSkeletton.h"

namespace world {

/** Gravity Based Homogeneous Branch Distribution */
class WORLDAPI_EXPORT TreeSkelettonHoGBasedWorker : public TreeSkelettonWorker {
    WORLD_WRITE_SUBCLASS_METHOD
public:
    TreeSkelettonHoGBasedWorker();

    void setStartWeight(double startWeight) { _startWeight = startWeight; }

    double getStartWeight() const { return _startWeight; }

    void setWeightLambda(double weightLambda) { _weightLambda = weightLambda; }

    void setSideRatio(double sideRatio) { _sideRatio = sideRatio; }

    void setSideSplit(u32 sideSplit) { _sideSplit = sideSplit; }

    void setEndSplit(u32 endSplit) { _endSplit = endSplit; }

    void setShapeFactor(double shapeFactor) { _shapeFactor = shapeFactor; }

    void setBendFactor(double bendFactor) { _bendFactor = bendFactor; }

    void processInstance(TreeInstance &tree, double resolution) override;

    TreeSkelettonHoGBasedWorker *clone() const override;

    void randomize() override;

    void write(WorldFile &wf) const override;

    void read(const WorldFile &wf) override;

private:
    std::mt19937 _rng;

    // Tree parameters;
    double _startWeight = 0.5;
    double _selfWeight = 0.15;
    /// A higher lambda means the branches have different weights.
    double _weightLambda = 2;
    /// Ratio of typical side branch weight against typical end branch weight
    double _sideRatio = 0.5;
    u32 _sideSplit = 6;
    u32 _endSplit = 3;

    /// The ratio between branch length and branch section.
    double _shapeFactor = 8;
    /// 0 means straight branches, 1 means folded.
    double _bendFactor = 0.5;

    // stop condition
    /// Maximum number of nodes from the root
    u32 _maxFork = 4;
    /// Minimum weight where the branches are dropped, relatively to startWeight
    double _minWeight = 0.0025;

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

#endif // WORLD_TREESKELETTONHOGBASEDWORKER_H
