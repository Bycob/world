//
// Created by louis on 21/04/17.
//

#include "Ground.h"

#include <memory>
#include <worldapi/terrain/TerrainManipulator.h>

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
    Map & map = world.getUniqueNode<Map>();

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
		applyMap(*pair.second, map);
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

void GroundGenerator::applyMap(Terrain & terrain, const Map & map, bool unapply) {
	std::unique_ptr<ITerrainManipulator> manipulator(ITerrainManipulator::createManipulator());

    int mapOx = map.getSizeX() / 2;
    int mapOy = map.getSizeY() / 2;
    int tX;// TODO récuperer les coordonnées du terrain
    int tY;

	uint32_t size = terrain.getSize();
	arma::mat bufferOffset(size, size);
	arma::mat bufferDiff(size, size);

	for (uint32_t x = 0 ; x < size ; x++) {
		for (uint32_t y = 0 ; y < size ; y++) {
            double mapX = tX + (double) x / size;
            double mapY = tY + (double) y / size;
			auto data = map.getReliefAt(mapX, mapY);

            if (unapply) {
                bufferOffset(x, y) = - data.first;
                bufferDiff(x, y) = 1 / data.second;
            }
            else {
                bufferOffset(x, y) = data.first;
                bufferDiff(x, y) = data.second;
            }
		}
	}

	if (unapply) {
		manipulator->applyOffset(terrain, bufferOffset, true);
		manipulator->multiply(terrain, bufferDiff);
	}
	else {
		manipulator->multiply(terrain, bufferDiff, true);
		manipulator->applyOffset(terrain, bufferOffset, true);
	}
}
