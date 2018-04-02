#include "Tree.h"

namespace world {
    Tree::Tree() : _trunkMaterial("trunk") {
        _trunkMaterial.setKd(0.5, 0.2, 0);
    }

    Tree::~Tree() {

    }

    void Tree::collectWholeObject(ICollector &collector) {
        using ItemKeys = ICollector::ItemKeys;

        Object3D mainPart(_trunkMesh);
        mainPart.setMaterialID("trunk");
        collector.addItem(ItemKeys::inObject(0), mainPart);

        // Material
        collector.addMaterial(ItemKeys::inObject(0), _trunkMaterial);
    }
}