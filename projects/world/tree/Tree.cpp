#include "Tree.h"

#include <vector>

#include "world/core/IResolutionModel.h"
#include "world/assets/Object3D.h"

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

void Tree::collect(ICollector &collector, const IResolutionModel &explorer) {

    // Generation
    if (!_generated) {
        generateBase();
    }

    // Collection
    Object3D mainPart(_trunkMesh);
    Object3D leaves(_leavesMesh);

    // Material
    Material leavesMat("leaves");
    leavesMat.setKd(0.4, 0.9, 0.4);

    if (collector.hasChannel<Object3D>()) {
        auto &objectsChannel = collector.getChannel<Object3D>();

        if (collector.hasChannel<Material>()) {
            auto &materialsChannel = collector.getChannel<Material>();

            mainPart.setMaterialID(ItemKeys::toString(ItemKeys::inObject(1)));
            leaves.setMaterialID(ItemKeys::toString(ItemKeys::inObject(2)));

            materialsChannel.put(ItemKeys::inObject(1), _trunkMaterial);
            materialsChannel.put(ItemKeys::inObject(2), leavesMat);
        }

        objectsChannel.put(ItemKeys::inObject(1), mainPart);
        objectsChannel.put(ItemKeys::inObject(2), leaves);
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