#include "WorldCollector.h"

#include "PrivateWorldCollector.h"

WorldCollector::WorldCollector()
        : _internal(new PrivateWorldCollector()) {

}

WorldCollector::~WorldCollector() {
    delete _internal;
}

void WorldCollector::reset() {
    _internal->_chunks.clear();
}

void WorldCollector::collect(World &world, ChunkNode &chunk) {

}

void WorldCollector::addObject3D(ObjectKey objkey, PartKey partkey, const Object3D &object) {

}

void WorldCollector::addMaterial(ObjectKey objkey, const Material &material) {

}