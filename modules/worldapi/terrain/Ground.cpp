//
// Created by louis on 21/04/17.
//

#include "Ground.h"

#include <memory>

#include "TerrainManipulator.h"
#include "TerrainGenerator.h"
#include "../world/FlatWorldCollector.h"

using namespace maths;

// Utility class
class GroundContext : public ITerrainGeneratorContext {
public:
    GroundContext(Ground& ground, int x, int y, int lvl) : _x(x), _y(y), _lvl(lvl), _ground(ground) {}

    bool neighbourExists(int x, int y) const override {
        return _ground.terrainExists(x + _x, y + _y, _lvl);
    }

    const Terrain& getNeighbour(int x, int y) const override {
        return _ground.terrain(x + _x, y + _y, _lvl);
    }
private:
    int _x, _y, _lvl;
    Ground &_ground;
};

class GroundCache {
public:
	GroundCache() : _terrains(), _mapUnitPerTerrain(2) {}

	/** Le nombre de pixel de map de relief par terrains de niveau 0  */
	double _mapUnitPerTerrain;
	std::map<vec2i, std::unique_ptr<Terrain>> _heightMap;
	std::map<vec2i, std::unique_ptr<Terrain>> _heightDiffMap;
	std::map<vec3i, std::unique_ptr<Terrain>> _terrains;
};

Ground::Ground() :
		  _cache(new GroundCache()),
		  _unitSize(4000), _maxAltitude(4000), _minAltitude(-2000) {
    _terrainGenerator = std::make_unique<PerlinTerrainGenerator>(0, 1, 4);
    _mapGenerator = std::make_unique<CustomWorldRMGenerator>();
}

Ground::~Ground() {
	delete _cache;
}

double Ground::observeAltitudeAt(double x, double y, int lvl) {
    double size = getTerrainSize(lvl);
    int xi = (int)floor(x / size);
    int yi = (int)floor(y / size);

    if (!terrainExists(xi, yi)) {
        generateTerrain(xi, yi, lvl);
    }

    const Terrain & terrain = const_cast<Ground*>(this)->terrain(xi, yi);
    return _minAltitude + getAltitudeRange() * terrain.getZInterpolated(x / size - xi, y / size - yi);
}

bool Ground::terrainExists(int x, int y, int lvl) const {
	return terrains().find({ x, y, lvl}) != terrains().end();
}

void Ground::collectChunk(FlatWorldCollector &collector, FlatWorld &world, WorldZone &chunkNode) {
    generateZone(world, chunkNode);
    Chunk& chunk = chunkNode.chunk();

    int lvl = 0;

    // Find terrains to generate
    double terrainSize = getTerrainSize(lvl);
    vec3d lower = chunk.getOffset() / terrainSize;
    vec3d upper = lower + chunk.getSize() / terrainSize;

    for (int x = (int) floor(lower.x) ; x < ceil(upper.x) ; x++) {
        for (int y = (int) floor(lower.y) ; y < ceil(upper.y) ; y++) {
            collector.addTerrain(getTerrainDataId(x, y, lvl), terrain(x, y, lvl));
        }
    }
}

long Ground::getTerrainDataId(int x, int y, int lvl) const {
	uint64_t id = (uint64_t)(x & 0x0FFFFFFF) 
		+ ((uint64_t) (y & 0x0FFFFFFF) << 24)
		+ ((uint64_t) (lvl & 0xFF) << 48);
	return id;
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

Terrain & Ground::terrainAt(double x, double y, int lvl) {
    int xi = (int) floor(x / _unitSize);
    int yi = (int) floor(y / _unitSize);

    return terrain(xi, yi);
}

double Ground::getTerrainSize(int lvl) const {
    return _unitSize * pow(2, - lvl); // TODO utiliser exponentiation rapide
}

void Ground::generateZone(FlatWorld &world, WorldZone &zone) {
    Chunk& chunk = zone.chunk();

    // Calculate lvl for the given chunk
    int lvl = 0;

    // Find terrains to generate
    // TODO Generate all the terrains with a higher level of detail
    double terrainSize = getTerrainSize(lvl);
    vec3d lower = chunk.getOffset() / terrainSize;
    vec3d upper = lower + chunk.getSize() / terrainSize;

    for (int x = (int) floor(lower.x) ; x < ceil(upper.x) ; x++) {
        for (int y = (int) floor(lower.y) ; y < ceil(upper.y) ; y++) {
            if (!terrainExists(x, y, lvl)) {
                generateTerrain(x, y, lvl);
            }
        }
    }
}

void Ground::generateTerrain(int x, int y, int lvl) {
    // Map turned off
    std::vector<vec3i> unmapped;
    unmapped.emplace_back(x - 1, y, lvl);
    unmapped.emplace_back(x + 1, y, lvl);
    unmapped.emplace_back(x, y - 1, lvl);
    unmapped.emplace_back(x, y + 1, lvl);

    for (auto & pos : unmapped) {
        if (terrainExists(pos.x, pos.y, pos.z))
            applyMap(pos.x, pos.y, pos.z, true);
    }

    // Terrain creation
    std::unique_ptr<Terrain> created = std::make_unique<Terrain>(129);
    _terrainGenerator->process(*created, GroundContext(*this, x, y, lvl));

    // Bounds
    double terrainSize = getTerrainSize(lvl);
    // TODO min altitude and max altitude also depend on lvl
    created->setBounds(terrainSize * x, terrainSize * y, _minAltitude,
                      terrainSize * (x + 1), terrainSize * (y + 1), _maxAltitude);

    _cache->_terrains[{x, y, lvl}] = std::move(created);

    // Map turned on
    unmapped.emplace_back(x, y, lvl);

    for (auto& pos : unmapped) {
        if (terrainExists(pos.x, pos.y, pos.z))
            applyMap(pos.x, pos.y, pos.z, false);
    }
}

void Ground::applyMap(int tX, int tY, int lvl, bool unapply) {

	// Terrain
	Terrain& terrain = *_cache->_terrains[{tX, tY, lvl}];
	uint32_t size = terrain.getSize();

    // Map
    vec2i mapLoc(0, 0);

    // (TODO optimize)

	// We generate the map if it's not done
    if (_cache->_heightMap.find(mapLoc) == _cache->_heightMap.end()) {
        _cache->_heightMap[mapLoc] = std::make_unique<Terrain>(100);
        _cache->_heightDiffMap[mapLoc] = std::make_unique<Terrain>(100);
        _mapGenerator->generate(*_cache->_heightMap[mapLoc], *_cache->_heightDiffMap[mapLoc]);
    }

    // Setup variables
	Terrain& heightMap = *_cache->_heightMap[{0, 0}];
	Terrain& diffMap = *_cache->_heightDiffMap[{0, 0}];
    const double ratio = _cache->_mapUnitPerTerrain / heightMap.getSize();
    const double mapOx = 0.5 + tX * ratio;
	const double mapOy = 0.5 + tY * ratio;

    const double offsetCoef = 0.5;
    const double diffCoef = 1 - offsetCoef;

    //std::cout << (unapply ? "unapply " : "apply ") << "to" << tX << ", " << tY << std::endl;

    std::unique_ptr<ITerrainManipulator> manipulator(ITerrainManipulator::createManipulator());

    arma::mat bufferOffset(size, size);
    arma::mat bufferDiff(size, size);

    for (uint32_t x = 0; x < size; x++) {
        for (uint32_t y = 0; y < size; y++) {
            double mapX = mapOx + ((double)x / (size - 1)) * ratio;
            double mapY = mapOy + ((double)y / (size - 1)) * ratio;

            double offset = offsetCoef * heightMap.getZInterpolated(mapX, mapY);
            double diff = diffMap.getZInterpolated(mapX, mapY) * diffCoef;

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
    }
}
