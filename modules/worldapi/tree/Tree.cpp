#include "Tree.h"

#include <vector>
#include <core/IResolutionModel.h>

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

void Tree::collect(ICollector &collector, const IResolutionModel &explorer) {
    // Generation
    if (!_generated) {
        generateBase();
    }

    // Collection
    using ItemKeys = ICollector::ItemKeys;

    Object3D mainPart(_trunkMesh);
    mainPart.setMaterialID("trunk");
    collector.addItem(ItemKeys::inObject(0), mainPart);

    // Material
    collector.addMaterial(ItemKeys::inObject(0), _trunkMaterial);
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