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
        _internal->_disabled.clear();
    }

    void Collector::addItem(const ItemKey &key, const Object3D &item) {
        if (_internal->_disabled.find(key) != _internal->_disabled.end()) {
            return;
        }

        _internal->_items.emplace(key, std::make_unique<CollectorItem>(key, item));
    }

	bool Collector::hasItem(const ItemKey &key) const {
		return _internal->_items.find(key) != _internal->_items.end();
	}

	void Collector::removeItem(const ItemKey &key) {
		_internal->_items.erase(key);
	}

	void Collector::disableItem(const world::ICollector::ItemKey &key) {
        removeItem(key);
        _internal->_disabled.emplace(key);
    }

	void Collector::addMaterial(const ItemKey & key, const Material & material) {
        if (_internal->_disabled.find(key) != _internal->_disabled.end()) {
            return;
        }

		_internal->_items.at(key)->_internal->_materials.emplace(material.getName(), material);
	}

	void Collector::addTexture(const ItemKey &key, const std::string &texName, const Image &texture) {
        if (_internal->_disabled.find(key) != _internal->_disabled.end()) {
            return;
        }

        auto &texMap = _internal->_items.at(key)->_internal->_image;
        auto it = texMap.find(texName);

        if (it == texMap.end()) {
            texMap.emplace(texName, std::make_unique<Image>(texture));
        }
    }

    CollectorIterator Collector::iterateItems() {
        return CollectorIterator(*this);
    }

	void world::Collector::fillScene(Scene &scene) {
		for (auto it = iterateItems(); it.hasNext(); ++it) {
			const auto &item = (*it).second;

			auto object = item->getObject3D();

			// Material
			std::string materialID = object.getMaterialID();
			auto material = item->getMaterial(materialID);
			
			if (material) {
				Material addedMat = *material;

				// Change the name to have distinct materials
				// TODO we should not have to do that
				materialID = ItemKeys::toString((*it).first) + materialID;
				addedMat.setName(materialID);
				object.setMaterialID(materialID);

				// Textures
				auto texture = item->getTexture(material->getMapKd());
				if (texture) {
					scene.addTexture(texture->_uid, texture->_image);
					// TODO we should not have to change the uid.
					addedMat.setMapKd(texture->_uid);
				}

				scene.addMaterial(addedMat);
			}

			scene.addObject(object);
		}
	}

    // ==== COLLECTOR OBJECT PART

    CollectorItem::CollectorItem(const ICollector::ItemKey &key, const Object3D &object3D) :
            _internal(new PrivateCollectorItem(key, object3D)) {

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
		    // TODO search in higher levels
			return nullopt;
		}
	}

	optional<CollectorItem::texture> CollectorItem::getTexture(const std::string &key) const {
        auto it = _internal->_image.find(key);

        if (it != _internal->_image.end()) {
            std::string uniqueStr = ICollector::ItemKeys::toString(_internal->_myKey) + "/" + key;
            return texture{ uniqueStr, *it->second };
        }
        else {
            // TODO search in higher levels
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