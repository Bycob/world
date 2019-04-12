#include "TreeGroup.h"

#include "world/math/MathsHelper.h"
#include "world/assets/MeshOps.h"
#include "world/assets/Object3D.h"
#include "TreeSkelettonGenerator.h"
#include "TrunkGenerator.h"
#include "LeavesGenerator.h"

namespace world {

struct TreeData {
    u32 _id;
    vec3d _position;
    std::vector<Face> _trunkFaces;
    std::vector<Face> _leavesFaces;
    std::unique_ptr<Tree> _tree;


    TreeData(u32 id, const vec3d &position) : _id(id), _position(position) {}
};


class PTreeGroup {
public:
    bool _updateParity = false;
    std::vector<TreeData> _trees;
};

TreeGroup::TreeGroup() : _internal(new PTreeGroup()) {}

TreeGroup::~TreeGroup() {}

void TreeGroup::addTree(const vec3d &pos) {
    _internal->_trees.emplace_back(_internal->_trees.size(), pos);
}

void TreeGroup::collect(ICollector &collector,
                        const IResolutionModel &resolutionModel,
                        const ExplorationContext &ctx) {

    if (_trunksMesh.getVerticesCount() == 0 &&
        _leavesMesh.getVerticesCount() == 0) {
        regenerateGroup();
    }

    _trunksMesh.clearFaces();
    _leavesMesh.clearFaces();

    for (TreeData &treeData : _internal->_trees) {
        double resolution =
            resolutionModel.getResolutionAt(treeData._position, ctx);

        if (resolution > 5) {
            // Generate tree
            if (treeData._tree == nullptr) {
                treeData._tree = std::make_unique<Tree>();
                treeData._tree->setPosition3D(treeData._position);
                configTree(*treeData._tree);
            }

            collectChild(NodeKeys::fromUint(treeData._id), *treeData._tree,
                         collector, resolutionModel, ctx);

        } else if (resolution > 1) {
            // Add simplified version to group mesh
            for (auto &face : treeData._trunkFaces) {
                _trunksMesh.addFace(face);
            }

            for (auto &face : treeData._leavesFaces) {
                _leavesMesh.addFace(face);
            }
        }
    }

    // Collect group meshes
    if (_trunksMesh.getFaceCount() != 0 || _leavesMesh.getFaceCount() != 0) {

        MeshOps::recalculateNormals(_trunksMesh);
        MeshOps::recalculateNormals(_leavesMesh);

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
            std::string prefix{
                (_internal->_updateParity = !_internal->_updateParity) ? "u"
                                                                       : ""};
            objChannel.put({prefix + "1"}, trunksObj, ctx);
            objChannel.put({prefix + "2"}, leavesObj, ctx);
        }
    }
}

void TreeGroup::regenerateGroup() {
    for (TreeData &treeData : _internal->_trees) {
        // trunk
        // TODO utiliser le générateur d'arbres pour générer une version low
        // poly du tronc avec peu de branches.
        vec3d trunkBottom = treeData._position;
        vec3d trunkTop = trunkBottom + vec3d{0, 0.3, 2};
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
            treeData._trunkFaces.emplace_back(ids[0]);
            treeData._trunkFaces.emplace_back(ids[1]);
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
                    treeData._leavesFaces.emplace_back(ids[0]);
                    treeData._leavesFaces.emplace_back(ids[1]);
                }
            }
        }
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