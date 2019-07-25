#include "Tree.h"

#include <vector>

#include "world/core/IResolutionModel.h"
#include "world/assets/SceneNode.h"

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

void Tree::collect(ICollector &collector, const IResolutionModel &explorer,
                   const ExplorationContext &ctx) {

    // Generation
    if (!_generated) {
        generateBase();
    }

    // Collection
    SceneNode trunk(ctx({"1"}).str());
    SceneNode leaves(ctx({"2"}).str());

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

        meshChannel.put({"1"}, _trunkMesh, ctx);
        meshChannel.put({"2"}, _leavesMesh, ctx);

        objectsChannel.put({"1"}, trunk, ctx);
        objectsChannel.put({"2"}, leaves, ctx);
    }
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