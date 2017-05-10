//
// Created by louis on 21/04/17.
//

#include "Ground.h"

#include <memory>

#include "Map.h"
#include "../terrain/TerrainGenerator.h"

using namespace maths;

INIT_WORLD_NODE_TYPE(Ground, "ground", true)

class GroundCache {
public:
	GroundCache() : _terrains() {}

	std::map<std::pair<int, int>, std::unique_ptr<Terrain>> _terrains;
};

Ground::Ground(const World * world)
        : WorldNode(world, WORLD_NODE_TYPE(Ground)),
		  _cache(new GroundCache()),
		  _unitSize(500), _maxAltitude(4000), _minAltitude(-2000) {

}

Ground::~Ground() {
	delete _cache;
}

bool Ground::isTerrainGenerated(int x, int y, int lvl) const {
	if (terrains().find(std::make_pair(x, y)) != terrains().end()) {
		return true;
	}

	if (_directory.hasFile(getTerrainDataId(x, y, lvl))) {
		return true;
	}

	return false;
}

const Terrain & Ground::getTerrain(int x, int y) const {
	return const_cast<Ground*>(this)->terrainAt(x, y);
}

std::string Ground::getTerrainDataId(int x, int y, int lvl) const {
	uint64_t id = (uint64_t)(x & 0x0FFFFFFF) 
		+ (uint64_t)(y & 0x0FFFFFFF) << 24 
		+ (uint64_t)(lvl & 0xFF) << 48;
	return std::to_string(id);
}

std::map<std::pair<int, int>, std::unique_ptr<Terrain>>& Ground::terrains() const {
	return _cache->_terrains;
}

Terrain & Ground::terrainAt(int x, int y) {
	throw nullptr;
}



GroundGenerator::GroundGenerator(WorldGenerator * parent)
        : WorldGenNode(parent) {

}

GroundGenerator::GroundGenerator(WorldGenerator * parent, const GroundGenerator & other)
	: WorldGenNode(parent), _generator(other._generator->clone()) {

}

void GroundGenerator::expand(World & world, const maths::vec3d & location) {
	Ground & ground = world.getUniqueNode<Ground>();

	float unit = ground._unitSize;
	float distance = _parent->getExpandRadius();
	float tdist = distance / unit;
	float tdist2 = tdist * tdist;

	int tx1 = (int) round((location.x - distance) / unit);
	int ty1 = (int) round((location.y - distance) / unit);
	int tx2 = (int) round((location.x + distance) / unit);
	int ty2 = (int) round((location.y + distance) / unit);

	// Creation des terrains
	std::map<std::pair<int, int>, std::unique_ptr<Terrain>> generated;
	
	for (int x = tx1; x <= tx2; x++) {
		for (int y = ty1; y <= ty1; y++) {
			if (x * x + y * y > tdist2 && !ground.isTerrainGenerated(x, y)) {
				generated[std::pair<int, int>(x, y)] = std::unique_ptr<Terrain>(_generator->generate());
			}
		}
	}

	// Fusionnement
	for (auto & pair : generated) {

	}

	for (auto & pair : generated) {
		applyMap(*pair.second);
		ground.terrains()[pair.first] = std::unique_ptr<Terrain>(pair.second.release());
	}
}

void GroundGenerator::addRequiredNodes(World &world) const {
	auto & map = requireUnique<Map>(world);
    auto & ground = requireUnique<Ground>(world);

	ground._unitSize = map.getUnitsPerPixel();
}

GroundGenerator* GroundGenerator::clone(WorldGenerator * newParent) {
    GroundGenerator * result = new GroundGenerator(newParent, *this);
    return result;
}

void GroundGenerator::applyMap(Terrain & terrain, bool unapply) {
	
}
