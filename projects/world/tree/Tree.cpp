#include "Tree.h"

#include <vector>

#include "world/core/IResolutionModel.h"
#include "world/assets/SceneNode.h"
#include "world/assets/MeshOps.h"

namespace world {
class PTree {
public:
    std::vector<std::unique_ptr<ITreeWorker>> _workers;
};

Tree::Tree() : _internal(new PTree()), _trunkMaterial("trunk") {
    _trunkMaterial.setKd(0.5, 0.2, 0);
}

Tree::~Tree() { delete _internal; }

void Tree::setup(const Tree &model) {
    _internal->_workers.clear();

    for (auto &worker : model._internal->_workers) {
        addWorkerInternal(worker->clone());
    }
}

const TreeSkeletton &Tree::getSkeletton() const { return _skeletton; }

TreeSkeletton &Tree::getSkeletton() { return _skeletton; }

const Mesh &Tree::getTrunkMesh() const { return _trunkMesh; }

Mesh &Tree::getTrunkMesh() { return _trunkMesh; }

const Mesh &Tree::getLeavesMesh() const { return _leavesMesh; }

Mesh &Tree::leavesMesh() { return _leavesMesh; }

void Tree::collect(ICollector &collector,
                   const IResolutionModel &resolutionModel,
                   const ExplorationContext &ctx) {

    double resolution = resolutionModel.getResolutionAt({}, ctx);

    const double SIMPLE_RES = 1;
    const double BASE_RES = 5;

    vec3d posOffset{};
    ItemKey trunkKey{"1"};
    ItemKey leavesKey{"2"};

    if (resolution < SIMPLE_RES) {
        return;
    } else if (resolution < BASE_RES) {
        if (_simpleTrunk.getVerticesCount() == 0)
            generateSimpleMeshes();

        trunkKey = {"s1"};
        leavesKey = {"s2"};
    } else {
        if (!_generated) {
            generateBase();
        }
        posOffset = ctx.hasEnvironment()
                        ? ctx.getEnvironment().findNearestFreePoint(
                              {}, {0, 0, 1}, resolution, ctx)
                        : vec3d{};
    }

    // Collection
    SceneNode trunk(ctx(trunkKey).str());
    SceneNode leaves(ctx(leavesKey).str());

    // Material
    Material leavesMat("leaves");
    leavesMat.setKd(0.4, 0.9, 0.4);

    if (collector.hasChannel<SceneNode>() && collector.hasChannel<Mesh>()) {
        auto &objectsChannel = collector.getChannel<SceneNode>();
        auto &meshChannel = collector.getChannel<Mesh>();

        if (collector.hasChannel<Material>()) {
            auto &materialsChannel = collector.getChannel<Material>();

            trunk.setMaterialID(ctx({"1"}).str());
            leaves.setMaterialID(ctx({"2"}).str());

            materialsChannel.put({"1"}, _trunkMaterial, ctx);
            materialsChannel.put({"2"}, leavesMat, ctx);
        }

        if (resolution < BASE_RES) {
            meshChannel.put(trunkKey, _simpleTrunk, ctx);
            meshChannel.put(leavesKey, _simpleLeaves, ctx);
        } else {
            meshChannel.put(trunkKey, _trunkMesh, ctx);
            meshChannel.put(leavesKey, _leavesMesh, ctx);
        }

        trunk.setPosition(posOffset);
        leaves.setPosition(posOffset);
        objectsChannel.put(trunkKey, trunk, ctx);
        objectsChannel.put(leavesKey, leaves, ctx);
    }
}

void Tree::generateSimpleMeshes() {
    // trunk
    // TODO utiliser le générateur d'arbres pour générer une version low
    // poly du tronc avec peu de branches.
    vec3d trunkBottom{};
    vec3d trunkTop = trunkBottom + vec3d{0, 0.3, 2};
    double trunkRadius = 0.2;

    for (int i = 0; i < 3; ++i) {
        double angle = M_PI * 2 * i / 3;
        vec3d shift{cos(angle) * trunkRadius, sin(angle) * trunkRadius, 0};
        _simpleTrunk.newVertex(trunkBottom + shift);
        _simpleTrunk.newVertex(trunkTop + shift);
        int ids[][3] = {{2 * i, (2 * i + 2) % 6, 2 * i + 1},
                        {(2 * i + 2) % 6, (2 * i + 2) % 6 + 1, 2 * i + 1}};
        _simpleTrunk.newFace(ids[0]);
        _simpleTrunk.newFace(ids[1]);
    }


    // leaves
    // TODO utiliser l'algorithme "QuickHull" pour générer des enveloppes
    // convexes
    vec3d leavesCenter = trunkTop;
    float radius[] = {0.8f, 1.5f, 1.2f, 0.4f};
    float height[] = {-0.35f, -0.05f, 0.5f, 0.9f};

    const int ringCount = 4;
    const int segmentCount = 7;

    for (int i = 0; i < ringCount; ++i) {
        for (int j = 0; j < segmentCount; ++j) {
            double angle = M_PI * 2 * j / segmentCount;
            vec3d vert =
                leavesCenter + vec3d{cos(angle) * radius[i],
                                     sin(angle) * radius[i], height[i]};
            _simpleLeaves.newVertex(vert);

            if (i != ringCount - 1) {
                int ringOffset = segmentCount * i;
                int totalOffset = ringOffset;
                int ids[][3] = {{ringOffset + j, ringOffset + (j + 1) % 7,
                                 ringOffset + segmentCount + j},
                                {ringOffset + (j + 1) % 7,
                                 ringOffset + segmentCount + (j + 1) % 7,
                                 ringOffset + segmentCount + j}};
                _simpleLeaves.newFace(ids[0]);
                _simpleLeaves.newFace(ids[1]);
            }
        }
    }

    MeshOps::recalculateNormals(_simpleTrunk);
    MeshOps::recalculateNormals(_simpleLeaves);
}

void Tree::generateBase() {
    for (auto &worker : _internal->_workers) {
        worker->process(*this);
    }

    _generated = true;
}

void Tree::addWorkerInternal(ITreeWorker *worker) {
    _internal->_workers.push_back(std::unique_ptr<ITreeWorker>(worker));
}
} // namespace world