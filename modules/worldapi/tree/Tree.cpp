#include "Tree.h"

#include "../world/WorldCollector.h"

Tree::Tree() {

}

Tree::~Tree() {

}

void Tree::collectWholeObject(WorldZone &zone, CollectorObject &collector) {
    Object3D mainPart(_trunkMesh);
    collector.putPart(0, mainPart);
}