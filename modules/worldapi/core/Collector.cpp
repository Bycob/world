#include "Collector.h"
#include "PrivateCollector.h"

#include "Chunk.h"

namespace world {

    // ==== COLLECTOR

    Collector::Collector()
            : _internal(new PrivateCollector()) {

    }

    Collector::~Collector() {
        delete _internal;
    }

    void Collector::reset() {
        _internal->_items.clear();
    }

    void Collector::addItem(const ItemKey &key, const Object3D &item) {
        _internal->_items.emplace(key, std::make_unique<CollectorItem>(item));
    }

	bool Collector::hasItem(const ItemKey &key) const {
		return _internal->_items.find(key) != _internal->_items.end();
	}

	void Collector::removeItem(const ItemKey &key) {
		_internal->_items.erase(key);
	}

	void Collector::addMaterial(const ItemKey & key, const Material & material) {
		_internal->_items.at(key)->_internal->_materials.emplace(material.getName(), material);
	}

    CollectorIterator Collector::iterateItems() {
        return CollectorIterator(*this);
    }

    // ==== COLLECTOR OBJECT PART

    CollectorItem::CollectorItem(const Object3D &object3D) :
            _internal(new PrivateCollectorItem(object3D)) {

    }

    CollectorItem::~CollectorItem() {
        delete _internal;
    }

    const Object3D &CollectorItem::getObject3D() const {
        return _internal->_object3D;
    }

    Object3D &CollectorItem::getObject3D() {
        return _internal->_object3D;
    }

	optional<const Material&> CollectorItem::getMaterial(const std::string & key) const {
		auto it = _internal->_materials.find(key);

		if (it != _internal->_materials.end()) {
			return it->second;
		}
		else {
			return nullopt;
		}
	}

    // ==== COLLECTOR ITERATOR

    CollectorIterator::CollectorIterator(Collector &collector)
            : _internal(new PrivateCollectorIterator()), _collector(collector) {
        _internal->_objectIt = _collector._internal->_items.begin();
    }

    CollectorIterator::CollectorIterator(const CollectorIterator &other)
            : _internal(new PrivateCollectorIterator()), _collector(other._collector) {
        _internal->_objectIt = other._internal->_objectIt;
    }

    CollectorIterator::~CollectorIterator() {
        delete _internal;
    }

    void CollectorIterator::operator++() {
        _internal->_objectIt++;
    }

    std::pair<Collector::ItemKey, CollectorItem* > CollectorIterator::operator*() {
        auto &object = *_internal->_objectIt;
        return std::make_pair(object.first, object.second.get());
    }

    bool CollectorIterator::hasNext() const {
        return _internal->_objectIt != _collector._internal->_items.end();
    }
}
