//
// Created by louis on 21/04/17.
//

#include "Ground.h"

#include <memory>

#include "TerrainGenerator.h"

using namespace maths;

class GroundCache {
public:
	GroundCache() : _terrains() {}

	std::map<vec2i, std::unique_ptr<Terrain>> _terrains;
};

Ground::Ground() :
		  _cache(new GroundCache()),
		  _unitSize(4000), _maxAltitude(4000), _minAltitude(-2000) {

}

Ground::~Ground() {
	delete _cache;
}

bool Ground::isTerrainGenerated(int x, int y, int lvl) const {
	if (terrains().find({ x, y }) != terrains().end()) {
		return true;
	}

	return false;
}

const Terrain & Ground::getTerrain(int x, int y) const {
	return const_cast<Ground*>(this)->terrain(x, y);
}

std::string Ground::getTerrainDataId(int x, int y, int lvl) const {
	uint64_t id = (uint64_t)(x & 0x0FFFFFFF) 
		+ ((uint64_t) (y & 0x0FFFFFFF) << 24)
		+ ((uint64_t) (lvl & 0xFF) << 48);
	return std::to_string(id);
}

std::map<vec2i, std::unique_ptr<Terrain>>& Ground::terrains() const {
	return _cache->_terrains;
}

Terrain & Ground::terrain(int x, int y) {
	auto terrain = _cache->_terrains.find({ x, y });
	
	if (terrain != _cache->_terrains.end()) {
		return *(*terrain).second;
	}

	throw std::runtime_error("Le terrain à l'emplacement (" + std::to_string(x) + ", " + std::to_string(y) + ") n'est pas généré.");
}

const Terrain & Ground::getTerrainAt(double x, double y, int lvl) const {
	int xi = (int) floor(x / _unitSize);
	int yi = (int) floor(y / _unitSize);

	return const_cast<Ground*>(this)->terrain(xi, yi);
}

double Ground::getAltitudeAt(double x, double y, int lvl) const {
	int xi = (int)floor(x / _unitSize);
	int yi = (int)floor(y / _unitSize);

	if (!isTerrainGenerated(xi, yi)) {
		return 0;
	}

	const Terrain & terrain = const_cast<Ground*>(this)->terrain(xi, yi);
	return _minAltitude + (_maxAltitude - _minAltitude) * terrain.getZInterpolated(x / _unitSize - xi, y / _unitSize - yi);
}

const std::vector<TerrainTile> Ground::getTerrainsFrom(const IPointOfView & from) const {
	std::vector<TerrainTile> result;

	iterateTerrainPos(from, [&] (int x, int y) {
		if (isTerrainGenerated(x, y)) {
			result.push_back({ &const_cast<Ground*>(this)->terrain(x, y), x, y });
		}
	});

	return result;
}

void Ground::iterateTerrainPos(const IPointOfView & from, const std::function<void(int, int)>& action) const {
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

			if (dx * dx + dy * dy <= tdist2) {
				action(x, y);
			}
		}
	}

}