#include "TreeGroup.h"

#include "world/math/MathsHelper.h"
#include "world/assets/MeshOps.h"
#include "world/assets/SceneNode.h"
#include "TreeSkelettonGenerator.h"
#include "TrunkGenerator.h"
#include "LeavesGenerator.h"

namespace world {

struct TreeData {
    u32 _id;
    vec3d _position;
    std::unique_ptr<Tree> _tree;


    TreeData(u32 id, const vec3d &position) : _id(id), _position(position) {}
};


class PTreeGroup {
public:
    std::vector<TreeData> _trees;
};

TreeGroup::TreeGroup() : _internal(new PTreeGroup()) {}

TreeGroup::~TreeGroup() { delete _internal; }

void TreeGroup::addTree(const vec3d &pos) {
    _internal->_trees.emplace_back((u32)_internal->_trees.size(), pos);
}

void TreeGroup::collect(ICollector &collector,
                        const IResolutionModel &resolutionModel,
                        const ExplorationContext &ctx) {

    if (resolutionModel.getResolutionAt({}, ctx) < 0.5) {
        return;
    }

    for (TreeData &treeData : _internal->_trees) {
        if (treeData._tree == nullptr) {
            treeData._tree = std::make_unique<Tree>();

            if (ctx.hasEnvironment()) {
                treeData._tree->setPosition3D(
                    ctx.getEnvironment().findNearestFreePoint(
                        treeData._position, {0, 0, 1}, 1, ctx));
            } else {
                treeData._tree->setPosition3D(treeData._position);
            }
            configTree(*treeData._tree);
        }

        // Generate tree
        collectChild(NodeKeys::fromUint(treeData._id), *treeData._tree,
                     collector, resolutionModel, ctx);
    }
}

void TreeGroup::configTree(Tree &tree) {
    auto &skeletton = tree.addWorker<TreeSkelettonGenerator>();
    skeletton.setRootWeight(TreeParamsd::gaussian(3, 0.2));
    skeletton.setForkingCount(
        TreeParamsi::WeightThreshold(0.15, TreeParamsi::uniform_int(3, 4)));
    skeletton.setInclination(
        TreeParamsd::PhiOffset(TreeParamsd::gaussian(0.2 * M_PI, 0.05 * M_PI)));
    skeletton.setTheta(
        TreeParamsd::UniformTheta(TreeParamsd::gaussian(0, 0.05 * M_PI)));
    skeletton.setSize(
        TreeParamsd::SizeFactor(TreeParamsd::uniform_real(0.5, 0.75)));

    tree.addWorker<TrunkGenerator>(12);
    tree.addWorker<LeavesGenerator>(0.2, 0.15);
}


} // namespace world