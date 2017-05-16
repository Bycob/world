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

typedef std::pair<int, int> id;

class GroundCache {
public:
	GroundCache() : _terrains() {}

	std::map<id, std::unique_ptr<Terrain>> _terrains;
};

Ground::Ground(const World * world)
        : WorldNode(world, WORLD_NODE_TYPE(Ground)),
		  _cache(new GroundCache()),
		  _unitSize(1000), _maxAltitude(4000), _minAltitude(-2000) {

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
	return const_cast<Ground*>(this)->terrain(x, y);
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

Terrain & Ground::terrain(int x, int y) {
	auto terrain = _cache->_terrains.find(std::make_pair(x, y));
	
	if (terrain != _cache->_terrains.end()) {
		return *(*terrain).second;
	}

	throw std::runtime_error("Le terrain à l'emplacement (" + std::to_string(x) + ", " + std::to_string(y) + ") n'est pas généré.");
}

const Terrain & Ground::getTerrainAt(double x, double y, int lvl) const {
	uint32_t xi = (uint32_t) floor(x / _unitSize);
	uint32_t yi = (uint32_t) floor(y / _unitSize);

	return const_cast<Ground*>(this)->terrain(xi, yi);
}

const std::vector<TerrainTile> Ground::getTerrainsFrom(const IPointOfView & from) const {
	std::vector<TerrainTile> result;

	// TODO duplication de code avec GroundGenerator::expand(...)
	vec3d location = from.getPosition();
	float distance = from.getHorizonDistance();
	
	float tdist = distance / _unitSize;
	float tdist2 = tdist * tdist;
	int centerX = (int)round(location.x / _unitSize);
	int centerY = (int)round(location.y / _unitSize);

	int tx1 = (int)round((location.x - distance) / _unitSize);
	int ty1 = (int)round((location.y - distance) / _unitSize);
	int tx2 = (int)round((location.x + distance) / _unitSize);
	int ty2 = (int)round((location.y + distance) / _unitSize);

	for (int x = tx1; x <= tx2; x++) {
		for (int y = ty1; y <= ty2; y++) {
			int dx = x - centerX;
			int dy = y - centerY;

			if (dx * dx + dy * dy <= tdist2 && isTerrainGenerated(x, y)) {
				result.push_back({ &const_cast<Ground*>(this)->terrain(x, y), x, y });
			}
		}
	}

	return result;
}



GroundGenerator::GroundGenerator(WorldGenerator * parent)
        : WorldGenNode(parent), _generator(std::make_unique<PerlinTerrainGenerator>()) {
	// TODO changer PerlinTerrainGenerator en un truc générique
	PerlinTerrainGenerator * generator = static_cast<PerlinTerrainGenerator*>(_generator.get());
	generator->setSize(129);
	generator->setFrequency(3);
}

GroundGenerator::GroundGenerator(WorldGenerator * parent, const GroundGenerator & other)
	: WorldGenNode(parent), _generator(other._generator->clone()) {

}

void GroundGenerator::expand(World & world, const IPointOfView & from) {
	vec3d location = from.getPosition();

	Ground & ground = world.getUniqueNode<Ground>();
    Map & map = world.getUniqueNode<Map>();

	float unit = ground._unitSize;
	float distance = from.getHorizonDistance();

	float tdist = distance / unit;
	float tdist2 = tdist * tdist;
	int centerX = (int)round(location.x / unit);
	int centerY = (int)round(location.y / unit);

	int tx1 = (int) round((location.x - distance) / unit);
	int ty1 = (int) round((location.y - distance) / unit);
	int tx2 = (int) round((location.x + distance) / unit);
	int ty2 = (int) round((location.y + distance) / unit);

	// Creation des terrains
	std::map<std::pair<int, int>, std::unique_ptr<Terrain>> generated;
	
	for (int x = tx1; x <= tx2; x++) {
		for (int y = ty1; y <= ty2; y++) {
			int dx = x - centerX;
			int dy = y - centerY;

			if (dx * dx + dy * dy <= tdist2 && !ground.isTerrainGenerated(x, y)) {
				generated[std::pair<int, int>(x, y)] = std::unique_ptr<Terrain>(_generator->generate());
			}
		}
	}

	// Fusionnement
	std::map<std::pair<int, int>, TerrainTile> joinedY;

	// Le joint y doit se faire avant le joint x
	// TODO simplifier l'algorithme suivant ?
	// join y
	for (auto & pair : generated) {
		int x = pair.first.first;
		int y = pair.first.second;
		Terrain& terrain = *pair.second;

		// Détermination du deuxième terrain à joindre
		// Fusion en dessus
		std::pair<int, int> key1(x, y + 1);
		auto terrain2 = generated.find(key1);

		if (terrain2 != generated.end()) {
			_generator->join(terrain, *(*terrain2).second, false, false);
		}
		else if ((terrain2 = ground.terrains().find(key1))
				 != ground.terrains().end()) {

			TerrainTile t2tile = {(*terrain2).second.get(), x, y + 1};
			applyMap(t2tile, map, true);
			_generator->join(terrain, *t2tile._terrain, false, false);
			joinedY[key1] = t2tile;
		}

		// Fusion en dessous si aucun terrain généré n'est présent
		std::pair<int, int> key2(x, y - 1);

		if (generated.find(key2) == generated.end() &&
				(terrain2 = ground.terrains().find(key2)) != ground.terrains().end()) {

			TerrainTile t2tile = {(*terrain2).second.get(), x, y - 1};
			applyMap(t2tile, map, true);
			_generator->join(*t2tile._terrain, terrain, false, false);
			joinedY[key2] = t2tile;
		}

		joinedY[{x, y}] = {&terrain, x, y};
	}

	// join x
	std::map<std::pair<int, int>, TerrainTile> joinedX;

	for (auto & pair : joinedY) {
		TerrainTile & tile = pair.second;
		Terrain & terrain = *tile._terrain;
		int x = tile._x;
		int y = tile._y;

		// Fusion x croissants
		std::pair<int, int> key1(x + 1, y);
		auto terrain2 = joinedY.find(key1);
		auto terrain2bis = ground.terrains().find(key1);

		if (terrain2 != joinedY.end()) {
			_generator->join(terrain, *(*terrain2).second._terrain, true, true);
		}
		else if (terrain2bis != ground.terrains().end()) {

			TerrainTile t2tile = {(*terrain2bis).second.get(), x + 1, y};
			applyMap(t2tile, map, true);
			_generator->join(terrain, *t2tile._terrain, true, true);
			joinedX[key1] = t2tile;
		}

		// Fusion x décroissants si aucun terrain généré n'est présent
		std::pair<int, int> key2(x - 1, y);

		if (joinedY.find(key2) == joinedY.end() &&
			(terrain2bis = ground.terrains().find(key2)) != ground.terrains().end()) {

			TerrainTile t2tile = {(*terrain2bis).second.get(), x - 1, y};
			applyMap(t2tile, map, true);
			_generator->join(*t2tile._terrain, terrain, true, true);
			joinedX[key2] = t2tile;
		}

		joinedX[{tile._x, tile._y}] = tile;
	}

	// Application de la carte
	for (auto & pair : joinedX) {
		applyMap(pair.second, map);
	}

	// Ajout
	for (auto & pair : generated) {
		TerrainTile tile = { pair.second.get(), pair.first.first, pair.first.second };
		applyMap(tile, map);
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

void GroundGenerator::applyMap(TerrainTile & tile, const Map & map, bool unapply) {
	Terrain & terrain = *tile._terrain;
	int tX = tile._x;
	int tY = tile._y;

	std::unique_ptr<ITerrainManipulator> manipulator(ITerrainManipulator::createManipulator());

    int mapOx = map.getSizeX() / 2;
    int mapOy = map.getSizeY() / 2;

	uint32_t size = terrain.getSize();
	arma::mat bufferOffset(size, size);
	arma::mat bufferDiff(size, size);

	for (uint32_t x = 0 ; x < size ; x++) {
		for (uint32_t y = 0 ; y < size ; y++) {
            double mapX = tX + mapOx + (double) x / size;
            double mapY = tY + mapOy + (double) y / size;
			auto data = map.getReliefAt(mapX, mapY);
			
            if (unapply) {
                bufferOffset(x, y) = - 0.5 * data.first;
                bufferDiff(x, y) = 1 / (data.second * 0.5);
            }
            else {
                bufferOffset(x, y) = 0.5 * data.first;
                bufferDiff(x, y) = 0.5 * data.second;
            }
		}
	}

	if (unapply) {
		manipulator->applyOffset(terrain, bufferOffset, true);
		manipulator->multiply(terrain, bufferDiff, true);
	}
	else {
		manipulator->multiply(terrain, bufferDiff, true);
		manipulator->applyOffset(terrain, bufferOffset, true);
	}
}
