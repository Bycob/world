#include "Collector.h"

#include "PrivateCollector.h"

namespace world {

    // ==== COLLECTOR

    Collector::Collector()
            : _internal(new PrivateCollector()) {

    }

    Collector::~Collector() {
        delete _internal;
    }

    void Collector::reset() {
        _internal->_chunks.clear();
    }

    void Collector::collect(World &world, WorldZone &zone) {
        // We don't collect the same chunk twice
        if (_internal->_chunks.find(zone->getID()) != _internal->_chunks.end()) {
            return;
        }

        auto chunk = std::make_unique<PrivateCollectorChunk>(zone);

        zone->chunk().forEachObject([&](WorldObject &object) {
            auto collObj = std::make_unique<CollectorObject>(zone, object);
            ObjectKey newKey = std::make_pair(zone->getID(), chunk->_nextKey);
            chunk->_nextKey++;
            object.collectWholeObject(zone, *collObj);
            chunk->_objects.emplace(newKey, std::move(collObj));
        });

        _internal->_chunks.emplace(zone->getID(), std::move(chunk));
    }

    CollectorObject &Collector::getCollectedObject(const ObjectKey &key) {
        auto &chunk = _internal->_chunks.at(getChunkKey(key));
        return *chunk->_objects.at(key);
    }

    CollectorIterator Collector::iterateObjects() {
        return CollectorIterator(*this);
    }

    Collector::ChunkKey Collector::getChunkKey(ObjectKey key) {
        return key.first;
    }


    // ==== COLLECTOR OBJECT

    CollectorObject::CollectorObject(WorldZone &zone, WorldObject &object)
            : _internal(new PrivateCollectorObject(zone, object)) {

    }

    CollectorObject::~CollectorObject() {
        delete _internal;
    }

    void CollectorObject::putPart(const Collector::PartKey &key, const Object3D &object) {
        WorldZone &here = _internal->_zone;

        auto part = std::make_unique<CollectorObjectPart>(object);
        Object3D &obj = part->getObject3D();
        // TODO changer en getRelativeOffset(reference)
        obj.setPosition(obj.getPosition() + here->getAbsoluteOffset() + _internal->_object.getPosition3D());

        _internal->_parts.emplace(key, std::move(part));

    }

    CollectorObjectPart &CollectorObject::getPart(const Collector::PartKey &key) {
        return *_internal->_parts.at(key);
    }

    const Object3D &CollectorObject::getPartAsObject3D(const Collector::PartKey &key) const {
        return const_cast<CollectorObject *>(this)->getPart(key).getObject3D();
    }


    // ==== COLLECTOR OBJECT PART

    CollectorObjectPart::CollectorObjectPart(const Object3D &object3D) :
            _internal(new PrivateCollectorObjectPart(object3D)) {

    }

    CollectorObjectPart::~CollectorObjectPart() {
        delete _internal;
    }

    const Object3D &CollectorObjectPart::getObject3D() const {
        return _internal->_object3D;
    }

    Object3D &CollectorObjectPart::getObject3D() {
        return _internal->_object3D;
    }


    // ==== COLLECTOR ITERATOR

    CollectorIterator::CollectorIterator(Collector &collector)
            : _internal(new PrivateCollectorIterator()), _collector(collector) {
        _internal->_chunkIt = chunks().begin();
        _internal->_objectIt = objects().begin();

        while (_internal->_objectIt == objects().end()) {
            _internal->_chunkIt++;

            if (_internal->_chunkIt != chunks().end()) {
                _internal->_objectIt = objects().begin();
            } else {
                break;
            }
        }
    }

    CollectorIterator::CollectorIterator(const CollectorIterator &other)
            : _internal(new PrivateCollectorIterator()), _collector(other._collector) {
        _internal->_chunkIt = other._internal->_chunkIt;
        _internal->_objectIt = other._internal->_objectIt;
    }

    CollectorIterator::~CollectorIterator() {
        delete _internal;
    }

    void CollectorIterator::operator++() {
        _internal->_objectIt++;

        while (_internal->_objectIt == objects().end()) {
            _internal->_chunkIt++;

            if (_internal->_chunkIt != chunks().end()) {
                _internal->_objectIt = objects().begin();
            } else {
                break;
            }
        }
    }

    std::pair<Collector::ObjectKey, CollectorObject *> CollectorIterator::operator*() {
        auto &object = *_internal->_objectIt;
        return std::make_pair(object.first, object.second.get());
    }

    bool CollectorIterator::hasNext() const {
        return _internal->_chunkIt != chunks().end();
    }

    std::map<Collector::ObjectKey, std::unique_ptr<CollectorObject>> &CollectorIterator::objects() const {
        return (*_internal->_chunkIt).second->_objects;
    }

    std::map<Collector::ChunkKey, std::unique_ptr<PrivateCollectorChunk>> &CollectorIterator::chunks() const {
        return _collector._internal->_chunks;
    }
}
