#include "TreeGroup.h"

#include "world/math/MathsHelper.h"
#include "world/assets/MeshOps.h"
#include "world/assets/Object3D.h"
#include "TreeSkelettonGenerator.h"
#include "TrunkGenerator.h"
#include "LeavesGenerator.h"

namespace world {

class PTreeGroup {
public:
    std::vector<std::unique_ptr<Tree>> _trees;
};

TreeGroup::TreeGroup() : _internal(new PTreeGroup()) {}

TreeGroup::~TreeGroup() {}

void TreeGroup::addTree(const vec3d &pos) { _treesPositions.push_back(pos); }

void TreeGroup::collect(ICollector &collector,
                        const IResolutionModel &resolutionModel,
                        const ExplorationContext &ctx) {

    // we choose the render mode of the tree group : 0:none / 1:together / 2:per
    // tree
    int renderMode = 0;

    for (vec3d &pos : _treesPositions) {
        double resolution = resolutionModel.getResolutionAt(pos, ctx);

        if (resolution > 15) {
            renderMode = 2;
            break;
        } else if (resolution > 1) {
            renderMode = 1;
        }
    }

    switch (renderMode) {
    case 1: // Together
    {
        // Generating if needed
        if (_trunksMesh.getFaceCount() == 0 &&
            _leavesMesh.getFaceCount() == 0) {
            regenerateGroup();
        }

        // Collecting
        if (collector.hasChannel<Object3D>()) {

            Object3D trunksObj(_trunksMesh);
            Object3D leavesObj(_leavesMesh);

            if (collector.hasChannel<Material>()) {
                Material trunkMaterial("trunk");
                trunkMaterial.setKd(0.5, 0.2, 0);
                Material leafMaterial("leaf");
                leafMaterial.setKd(0.4, 0.9, 0.4);

                auto &matChannel = collector.getChannel<Material>();
                trunksObj.setMaterialID(ctx.mutateKey({"1"}).str());
                leavesObj.setMaterialID(ctx.mutateKey({"2"}).str());

                matChannel.put({"1"}, trunkMaterial, ctx);
                matChannel.put({"2"}, leafMaterial, ctx);
            }

            auto &objChannel = collector.getChannel<Object3D>();
            objChannel.put({"1"}, trunksObj, ctx);
            objChannel.put({"2"}, leavesObj, ctx);
        }

        break;
    }
    case 2: // Tree by tree
        for (u32 i = 0; i < _treesPositions.size(); ++i) {
            if (_internal->_trees.size() <= i) {
                allocateTree(_treesPositions[i]);
            }

            Tree &tree = *_internal->_trees.at(i);
            collectChild(NodeKeys::fromUint(i), tree, collector,
                         resolutionModel, ctx);
        }

        break;
    }
}

void TreeGroup::regenerateGroup() {
    for (vec3d &pos : _treesPositions) {
        // trunk
        // TODO utiliser le générateur d'arbres pour générer une version low
        // poly du tronc avec peu de branches.
        vec3d trunkBottom = pos;
        vec3d trunkTop = pos + vec3d{0, 0.3, 2};
        double trunkRadius = 0.2;
        int startIndex = _trunksMesh.getVerticesCount();

        for (int i = 0; i < 3; ++i) {
            double angle = M_PI * 2 * i / 3;
            vec3d shift{cos(angle) * trunkRadius, sin(angle) * trunkRadius, 0};
            _trunksMesh.newVertex(trunkBottom + shift);
            _trunksMesh.newVertex(trunkTop + shift);
            int ids[][3] = {{startIndex + 2 * i, startIndex + (2 * i + 2) % 6,
                             startIndex + 2 * i + 1},
                            {startIndex + (2 * i + 2) % 6,
                             startIndex + (2 * i + 2) % 6 + 1,
                             startIndex + 2 * i + 1}};
            _trunksMesh.newFace(ids[0]);
            _trunksMesh.newFace(ids[1]);
        }


        // leaves
        // TODO utiliser l'algorithme "QuickHull" pour générer des enveloppes
        // convexes
        vec3d leavesCenter = trunkTop;
        float radius[] = {0.8f, 1.5f, 1.2f, 0.4f};
        float height[] = {-0.35f, -0.05f, 0.5f, 0.9f};
        startIndex = _leavesMesh.getVerticesCount();

        const int ringCount = 4;
        const int segmentCount = 7;

        for (int i = 0; i < ringCount; ++i) {
            for (int j = 0; j < segmentCount; ++j) {
                double angle = M_PI * 2 * j / segmentCount;
                vec3d vert =
                    leavesCenter + vec3d{cos(angle) * radius[i],
                                         sin(angle) * radius[i], height[i]};
                _leavesMesh.newVertex(vert);

                if (i != ringCount - 1) {
                    int ringOffset = segmentCount * i;
                    int totalOffset = startIndex + ringOffset;
                    int ids[][3] = {{totalOffset + j, totalOffset + (j + 1) % 7,
                                     totalOffset + segmentCount + j},
                                    {totalOffset + (j + 1) % 7,
                                     totalOffset + segmentCount + (j + 1) % 7,
                                     totalOffset + segmentCount + j}};
                    _leavesMesh.newFace(ids[0]);
                    _leavesMesh.newFace(ids[1]);
                }
            }
        }
    }

    MeshOps::recalculateNormals(_trunksMesh);
    MeshOps::recalculateNormals(_leavesMesh);
}

void TreeGroup::allocateTree(const vec3d &position) {
    _internal->_trees.emplace_back(std::make_unique<Tree>());
    Tree &tree = *_internal->_trees.back();
    tree.setPosition3D(position);

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