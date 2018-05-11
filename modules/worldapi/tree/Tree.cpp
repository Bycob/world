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
    using ItemKeys = ICollector::ItemKeys;

    // Generation
    if (!_generated) {
        generateBase();
    }

    // Collection
    if (explorer.getResolutionAt(vec3d{0, 0, 0}) < 5) {
        // Collect a dummy mesh
        Mesh mesh;

        vec3d vertices[8] = {
            {-0.2, -0.2, 0},
            {-0.2, 0.2, 0},
            {0.2, 0.2, 0},
            {0.2, -0.2, 0},
            {-0.2, -0.2, 2},
            {-0.2, 0.2, 2},
            {0.2, 0.2, 2},
            {0.2, -0.2, 2}
        };

        vec3d normals[8] = {
                {-0.2, -0.2, 0},
                {-0.2, 0.2, 0},
                {0.2, 0.2, 0},
                {0.2, -0.2, 0},
                {-0.2, -0.2, 0},
                {-0.2, 0.2, 0},
                {0.2, 0.2, 0},
                {0.2, -0.2, 0}
        };

        int indices[8][3] = {
            { 0, 4, 5},
            { 0, 1, 5 },
            { 1, 5, 6 },
            { 1, 2, 6 },
            { 2, 6, 7 },
            { 2, 3, 7 },
            { 3, 7, 4 },
            { 3, 0, 4 }
        };

        for (int i = 0; i < 8; ++i) {
            mesh.newVertex(vertices[i], normals[i]);
        } for (int i = 0; i < 8; ++i) {
            mesh.newFace(indices[i]);
        }

        Object3D dummy(mesh);
        dummy.setMaterialID("trunk");
        collector.addItem(ItemKeys::inObject(1), dummy);

        // Material
        collector.addMaterial(ItemKeys::inObject(1), _trunkMaterial);
    }
    else {
        Object3D mainPart(_trunkMesh);
        mainPart.setMaterialID("trunk");
        collector.addItem(ItemKeys::inObject(0), mainPart);

        // Material
        collector.addMaterial(ItemKeys::inObject(0), _trunkMaterial);
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