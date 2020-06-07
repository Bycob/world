
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

    // balancing
    u32 _itcount = 5;
    double _step = 0.2;


    void forkNode(SkelettonNode<TreeInfo> *node);

    void initBranch(SkelettonNode<TreeInfo> *node); // TODO add the other
                                                    // parameters

    void balanceSphere(std::vector<vec3d> &dirs,
                       const std::vector<double> &weights);

    void balanceSides(std::vector<vec3d> &pos, std::vector<vec3d> &dir,
                      const std::vector<double> &weights);
};
} // namespace world

#endif // WORLD_TREESKELETTONWORKER_H
