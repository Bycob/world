#include "Tree.h"

#include "worldapi/world/Collector.h"

namespace world {
    Tree::Tree() {

    }

    Tree::~Tree() {

    }

    void Tree::collectWholeObject(WorldZone &zone, CollectorObject &collector) {
        Object3D mainPart(_trunkMesh);
        collector.putPart(0, mainPart);
    }
}