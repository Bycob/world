#include "FlatWorldCollector.h"

namespace world {

    using TerrainKey = FlatWorldCollector::TerrainKey;

    class PrivateTerrainIterator {
    public:
        std::map<TerrainKey, Terrain>::iterator _it;
    };

    TerrainIterator::TerrainIterator(FlatWorldCollector &collector)
            : _internal(new PrivateTerrainIterator()),
              _collector(collector) {

        _internal->_it = _collector._terrains.begin();
    }

    TerrainIterator::~TerrainIterator() {
        delete _internal;
    }

    void TerrainIterator::operator++() {
        _internal->_it++;
    }

    std::pair<TerrainKey, Terrain *> TerrainIterator::operator*() {
        auto &p = *_internal->_it;
        return std::make_pair(p.first, &p.second);
    }

    bool TerrainIterator::hasNext() const {
        return _internal->_it != _collector._terrains.end();
    }


    FlatWorldCollector::FlatWorldCollector() = default;

    FlatWorldCollector::~FlatWorldCollector() = default;

    void FlatWorldCollector::reset() {
        Collector::reset();

        _terrains.clear();
        _disabledTerrains.clear();
    }

//#define COLLECT_MESH

	inline ICollector::ItemKey terrainToItem(const TerrainKey &key) {
		return ICollector::ItemKeys::from(std::string("_") + key, ObjectKeys::defaultKey(), 0);
	}

    void FlatWorldCollector::addTerrain(TerrainKey key, const Terrain &terrain) {
        if (_disabledTerrains.find(key) != _disabledTerrains.end()) {
            return;
        }

#ifdef COLLECT_MESH
		ItemKey itemKey = terrainToItem(key);

		if (!hasItem(itemKey)) {
			// Relocate the terrain
			auto & bbox = terrain.getBoundingBox();
			vec3d offset = bbox.getLowerBound();
			vec3d size = bbox.getUpperBound() - offset;

			std::shared_ptr<Mesh> mesh(terrain.convertToMesh(0, 0, 0, size.x, size.y, size.z));
			Object3D object(mesh);
			object.setPosition(offset);
			addItemUnsafe(itemKey, object);
		}
#else
		auto it = _terrains.find(key);

        if (it == _terrains.end()) {
            _terrains.emplace_hint(it, key, terrain);
        }
#endif
    }

    void FlatWorldCollector::disableTerrain(TerrainKey key) {

#ifdef COLLECT_MESH
		ItemKey itemKey = terrainToItem(key);
		removeItem(itemKey);
#else
		_terrains.erase(key);
#endif

        _disabledTerrains.insert(key);
    }

    TerrainIterator FlatWorldCollector::iterateTerrains() {
        return TerrainIterator(*this);
    }
}
