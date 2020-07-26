
#ifndef WORLD_TREESKELETTONWORKER_H
#define WORLD_TREESKELETTONWORKER_H

#include "world/core/WorldConfig.h"

#include "ITreeWorker.h"
#include "TreeSkeletton.h"

namespace world {

class WORLDAPI_EXPORT TreeSkelettonWorker : public ITreeWorker {
    WORLD_WRITE_SUBCLASS_METHOD
public:
    TreeSkelettonWorker();

    void processInstance(TreeInstance &tree, double resolution) override;

    TreeSkelettonWorker *clone() const override;

private:
    std::mt19937 _rng;

    // Tree parameters;
    double _weightLambda = 2;
    double _minWeight = 0.002;

    // balancing
    u32 _itcount = 5;
    double _step = 0.2;


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
