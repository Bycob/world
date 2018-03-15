#include "WorldCollector.h"

#include "PrivateWorldCollector.h"

// ==== COLLECTOR

WorldCollector::WorldCollector()
        : _internal(new PrivateWorldCollector()) {

}

WorldCollector::~WorldCollector() = default;

void WorldCollector::reset() {
    _internal->_chunks.clear();
}

void WorldCollector::collect(World &world, WorldZone &zone) {
    // We don't collect the same chunk twice
    if (_internal->_chunks.find(zone->getID()) != _internal->_chunks.end()) {
        return;
    }

    auto chunk = std::make_unique<PrivateCollectorChunk>(zone);

    zone->chunk().forEachObject([&] (WorldObject & object) {
        auto collObj = std::make_unique<CollectorObject>(zone, object);
        ObjectKey newKey = std::make_pair(zone->getID(), chunk->_nextKey);
        chunk->_nextKey++;
        object.collectWholeObject(zone, *collObj);
        chunk->_objects.emplace(newKey, std::move(collObj));
    });

    _internal->_chunks.emplace(zone->getID(), std::move(chunk));
}

CollectorObject& WorldCollector::getCollectedObject(const ObjectKey &key) {
    auto &chunk = _internal->_chunks.at(getChunkKey(key));
    return *chunk->_objects.at(key);
}

CollectorIterator WorldCollector::iterateObjects() {
    return CollectorIterator(*this);
}

WorldCollector::ChunkKey WorldCollector::getChunkKey(ObjectKey key) {
    return key.first;
}


// ==== COLLECTOR OBJECT

CollectorObject::CollectorObject(WorldZone &zone, WorldObject &object)
        : _internal(std::make_unique<PrivateCollectorObject>(zone, object)) {

}

void CollectorObject::putPart(const WorldCollector::PartKey &key, const Object3D &object) {
    WorldZone &here = _internal->_zone;

    auto part = std::make_unique<CollectorObjectPart>(object);
    Object3D &obj = part->getObject3D();
    // TODO changer en getRelativeOffset(reference)
    obj.setPosition(obj.getPosition() + here->getAbsoluteOffset() + _internal->_object.getPosition3D());

    _internal->_parts.emplace(key, std::move(part));

}

CollectorObjectPart& CollectorObject::getPart(const WorldCollector::PartKey &key) {
    return *_internal->_parts.at(key);
}

const Object3D& CollectorObject::getPartAsObject3D(const WorldCollector::PartKey &key) const {
    return const_cast<CollectorObject*>(this)->getPart(key).getObject3D();
}


// ==== COLLECTOR OBJECT PART

CollectorObjectPart::CollectorObjectPart(const Object3D &object3D) :
        _internal(std::make_unique<PrivateCollectorObjectPart>(object3D)) {

}

const Object3D& CollectorObjectPart::getObject3D() const {
    return _internal->_object3D;
}

Object3D& CollectorObjectPart::getObject3D() {
    return _internal->_object3D;
}


// ==== COLLECTOR ITERATOR

CollectorIterator::CollectorIterator(WorldCollector &collector)
        : _internal(new PrivateCollectorIterator()), _collector(collector) {
    _internal->_chunkIt = chunks().begin();
    _internal->_objectIt = objects().begin();

    while (_internal->_objectIt == objects().end()) {
        _internal->_chunkIt++;

        if (_internal->_chunkIt != chunks().end()) {
            _internal->_objectIt = objects().begin();
        }
        else {
            break;
        }
    }
}

CollectorIterator::CollectorIterator(const CollectorIterator &other)
        : _internal(new PrivateCollectorIterator()), _collector(other._collector) {
    _internal->_chunkIt = other._internal->_chunkIt;
    _internal->_objectIt = other._internal->_objectIt;
}

CollectorIterator::~CollectorIterator() = default;

void CollectorIterator::operator++() {
    _internal->_objectIt++;

    while (_internal->_objectIt == objects().end()) {
        _internal->_chunkIt++;

        if (_internal->_chunkIt != chunks().end()) {
            _internal->_objectIt = objects().begin();
        }
        else {
            break;
        }
    }
}

std::pair<WorldCollector::ObjectKey, CollectorObject*> CollectorIterator::operator*() {
    auto & object = *_internal->_objectIt;
    return std::make_pair(object.first, object.second.get());
}

bool CollectorIterator::hasNext() const {
    return _internal->_chunkIt != chunks().end();
}

std::map<WorldCollector::ObjectKey, std::unique_ptr<CollectorObject>>& CollectorIterator::objects() const {
    return (*_internal->_chunkIt).second->_objects;
}

std::map<WorldCollector::ChunkKey, std::unique_ptr<PrivateCollectorChunk>>& CollectorIterator::chunks() const {
    return _collector._internal->_chunks;
}

