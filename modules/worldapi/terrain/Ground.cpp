//
// Created by louis on 21/04/17.
//

#include "Ground.h"

#include <memory>

#include "TerrainManipulator.h"
#include "TerrainGenerator.h"

using namespace maths;

class GroundCache {
public:
	GroundCache() : _terrains() {}

	std::map<vec2i, std::unique_ptr<Terrain>> _heightMap;
	std::map<vec2i, std::unique_ptr<Terrain>> _heightDiffMap;
	std::map<vec3i, std::unique_ptr<Terrain>> _terrains;
};

Ground::Ground() :
		  _cache(new GroundCache()),
		  _unitSize(4000), _maxAltitude(4000), _minAltitude(-2000) {

}

Ground::~Ground() {
	delete _cache;
}

bool Ground::isTerrainGenerated(int x, int y, int lvl) const {
	return terrains().find({ x, y, lvl}) != terrains().end();
}

const Terrain & Ground::getTerrain(int x, int y, int lvl) const {
	return const_cast<Ground*>(this)->terrain(x, y);
}

const Terrain & Ground::getTerrainAt(double x, double y, int lvl) const {
    int xi = (int) floor(x / _unitSize);
    int yi = (int) floor(y / _unitSize);

    return const_cast<Ground*>(this)->terrain(xi, yi);
}

std::string Ground::getTerrainDataId(int x, int y, int lvl) const {
	uint64_t id = (uint64_t)(x & 0x0FFFFFFF) 
		+ ((uint64_t) (y & 0x0FFFFFFF) << 24)
		+ ((uint64_t) (lvl & 0xFF) << 48);
	return std::to_string(id);
}

std::map<vec3i, std::unique_ptr<Terrain>>& Ground::terrains() const {
	return _cache->_terrains;
}

Terrain & Ground::terrain(int x, int y, int lvl) {
	auto terrain = _cache->_terrains.find({ x, y, lvl});
	
	if (terrain != _cache->_terrains.end()) {
		return *(*terrain).second;
	}

	throw std::runtime_error("Le terrain à l'emplacement (" + std::to_string(x) + ", " + std::to_string(y) + ") n'est pas généré.");
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

double Ground::getTerrainSize(int lvl) const {
    return _unitSize * pow(2, - lvl); // TODO utiliser exponentiation rapide
}

void Ground::generateChunk(FlatWorld &world, Chunk &chunk) {
    // Calculate lvl for the given chunk
    int lvl = 0;

    // Find terrains to generate
    double terrainSize = getTerrainSize(lvl);
    vec3d lower = chunk.getOffset() / terrainSize;
    vec3d upper = lower + chunk.getSize() / terrainSize;

    for (int x = (int) floor(lower.x) ; x < ceil(upper.x) ; x++) {
        for (int y = (int) floor(lower.y) ; y < ceil(upper.y) ; y++) {
            if (!isTerrainGenerated(x, y, lvl)) {
                generateTerrain(x, y, lvl);
            }
        }
    }
}

void Ground::generateTerrain(int x, int y, int lvl) {
    // Terrain creation
    _cache->_terrains[{x, y, lvl}] = std::unique_ptr<Terrain>(_terrainGenerator->generate());

    // Join
    // TODO

    // Generation of the height and heightDiff (TODO move to generator metadata)

}

void Ground::applyMap(int x, int y, int lvl, bool unapply) {
    /*const double offsetCoef = 0.5;
    const double diffCoef = 1 - offsetCoef;

    const double ratio = _metadata.unitsPerTerrain / _metadata.unitsPerMapPixel;

    Terrain & terrain = *tile._terrain;
    int tX = tile._x;
    int tY = tile._y;
    //std::cout << (unapply ? "unapply " : "apply ") << "to" << tX << ", " << tY << std::endl;

    std::unique_ptr<ITerrainManipulator> manipulator(ITerrainManipulator::createManipulator());

    int mapOx = map.getSizeX() / 2;
    int mapOy = map.getSizeY() / 2;

    uint32_t size = terrain.getSize();
    arma::mat bufferOffset(size, size);
    arma::mat bufferDiff(size, size);

    for (uint32_t x = 0; x < size; x++) {
        for (uint32_t y = 0; y < size; y++) {
            double mapX = mapOx + (tX + (double)x / size) * ratio;
            double mapY = mapOy + (tY + (double)y / size) * ratio;
            auto data = map.getReliefAt(mapX, mapY);

            double offset = offsetCoef * data.first;
            double diff = data.second * diffCoef;

            if (unapply) {
                bufferOffset(x, y) = -offset;
                bufferDiff(x, y) = diff > std::numeric_limits<double>::epsilon() ? 1 / diff : 0;
            }
            else {
                bufferOffset(x, y) = offset;
                bufferDiff(x, y) = diff;
            }
        }
    }

    if (unapply) {
        manipulator->applyOffset(terrain, bufferOffset);
        manipulator->multiply(terrain, bufferDiff);
    }
    else {
        manipulator->multiply(terrain, bufferDiff);
        manipulator->applyOffset(terrain, bufferOffset);
    }*/
}
