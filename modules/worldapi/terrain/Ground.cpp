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

// Idees d'ameliorations :
// - Au lieu d'apply et d'unapply la map et les terrains parents,
// conserver le terrain brut en cache aussi longtemps que nécessaire
// - Systeme de coordonnees semblable a celui du chunk system : les
// coordonnees sont relatives au niveau du dessus
// (Pour ce point, on generalisera peut-etre le system de chunk emboite
// dans une classe avec des templates)

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

    if (!terrainExists(xi, yi, lvl)) {
        generateTerrain(xi, yi, lvl);
    }

    const Terrain & terrain = const_cast<Ground*>(this)->terrain(xi, yi, lvl);
    return _minAltitude + getAltitudeRange() * terrain.getZInterpolated(x / size - xi, y / size - yi);
}

double Ground::observeAltitudeAt(WorldZone zone, double x, double y) {
    int lvl = getLevelForChunk(zone);
    std::cout << lvl << " " << zone->getID().getLOD() << std::endl;
    vec3d offset = zone->getAbsoluteOffset();
    return observeAltitudeAt(offset.x + x, offset.y + y, lvl);
}

void Ground::collectZone(FlatWorldCollector &collector, FlatWorld &world, WorldZone &zone) {
    generateZone(world, zone);
    Chunk& chunk = zone->chunk();

    int lvl = getLevelForChunk(zone);

    // Find terrains to generate
    double terrainSize = lvl == 0 ? getTerrainSize(lvl) : getTerrainSize(lvl - 1);
    vec3d lower = zone->getAbsoluteOffset() / terrainSize;
    vec3d upper = lower + chunk.getSize() / terrainSize;

    for (int x = (int) floor(lower.x) ; x < ceil(upper.x) ; x++) {
        for (int y = (int) floor(lower.y) ; y < ceil(upper.y) ; y++) {

            if (lvl == 0) {
                collector.addTerrain(getTerrainDataId(x, y, lvl), terrain(x, y, lvl));
            }
            else {
                replaceTerrain(x, y, lvl - 1, collector);
            }
        }
    }
}
std::map<vec3i, std::unique_ptr<Terrain>>& Ground::terrains() const {
	return _cache->_terrains;
}

Terrain & Ground::terrain(int x, int y, int lvl) {
	auto terrain = _cache->_terrains.find({ x, y, lvl});
	
	if (terrain != _cache->_terrains.end()) {
		return *(*terrain).second;
	}

	throw std::runtime_error("Le terrain à l'emplacement (" + std::to_string(x) + ", " + std::to_string(y) + " ; " + std::to_string(lvl) + ") n'est pas généré.");
}

Terrain & Ground::terrainAt(double x, double y, int lvl) {
    int xi = (int) floor(x / _unitSize);
    int yi = (int) floor(y / _unitSize);

    return terrain(xi, yi);
}

bool Ground::terrainExists(int x, int y, int lvl) const {
    return terrains().find({ x, y, lvl}) != terrains().end();
}

void Ground::replaceTerrain(int xp, int yp, int lvl, FlatWorldCollector &collector) {
    collector.disableTerrain(getTerrainDataId(xp, yp, lvl));

    for (int x = xp * _factor; x < xp * _factor + _factor ; x++) {
        for (int y = yp * _factor; y < yp * _factor + _factor; y++) {
            if (!terrainExists(x, y, lvl + 1)) {
                generateTerrain(x, y, lvl + 1);
            }
            collector.addTerrain(getTerrainDataId(x, y, lvl + 1), terrain(x, y, lvl + 1));
        }
    }
}

long Ground::getTerrainDataId(int x, int y, int lvl) const {
    uint64_t id = (uint64_t)(x & 0x0FFFFFFF)
                  + ((uint64_t) (y & 0x0FFFFFFF) << 24)
                  + ((uint64_t) (lvl & 0xFF) << 48);
    return id;
}


double Ground::getTerrainSize(int lvl) const {
    return _unitSize * powi((double) _factor, - lvl);
}

int Ground::getLevelForChunk(const WorldZone &zone) const {
    double maxDetailSize = zone->getChunk().getMaxDetailSize(); // .getMinDetailSize();
    return clamp((int) (log(_unitSize / (maxDetailSize * _terrainRes)) / log(_factor)), 0, _maxLOD);
    // Formula obtained from the equation : getTerrainSize(lvl) / terrainRes = maxDetailSize
}

maths::vec3i Ground::getParentId(const maths::vec3i &childId) const {
    return {
            (childId.x - mod(childId.x, _factor)) / _factor,
            (childId.y - mod(childId.y, _factor)) / _factor,
            childId.z - 1
    };
}

void Ground::generateZone(FlatWorld &world, WorldZone &zone) {
    Chunk& chunk = zone->chunk();

    // Calculate lvl for the given chunk
    int lvl = getLevelForChunk(zone);

    // Find terrains to generate
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
        if (terrainExists(pos.x, pos.y, pos.z)) {
            applyPreviousLayer(pos.x, pos.y, pos.z, true);
        }
    }

    // Terrain creation
    std::unique_ptr<Terrain> created = std::make_unique<Terrain>(_terrainRes);
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
        if (terrainExists(pos.x, pos.y, pos.z)) {
            applyPreviousLayer(pos.x, pos.y, pos.z, false);
        }
    }
}

void Ground::applyPreviousLayer(int x, int y, int lvl, bool unapply) {
    if (lvl == 0) {
        applyMap(x, y, lvl, unapply);
    } else {
        applyParent(x, y, lvl, unapply);
    }
}

void Ground::applyMap(int tX, int tY, int lvl, bool unapply) {

	// Terrain
	Terrain& terrain = this->terrain(tX, tY, lvl);
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

void Ground::applyParent(int tX, int tY, int lvl, bool unapply) {
    vec3i childId{tX, tY, lvl};
    Terrain& child = this->terrain(tX, tY, lvl);
    int size = child.getSize();

    // Get parent. Generate it if needed
    auto parentId = getParentId(childId);

    if (!terrainExists(parentId.x, parentId.y, parentId.z)) {
        generateTerrain(parentId.x, parentId.y, parentId.z);
    }

    Terrain &parent = this->terrain(parentId.x, parentId.y, parentId.z);

    // Useful variables
    const double parentProp = 0.97;
    const double childProp = 1. - parentProp;
    const double oX = (double) mod(tX, _factor) / _factor;
    const double oY = (double) mod(tY, _factor) / _factor;
    const double ratio = 1. / _factor;

    std::unique_ptr<ITerrainManipulator> manipulator(ITerrainManipulator::createManipulator());

    arma::mat bufferParent(size, size);

    for (int x = 0; x < size ; x++) {
        for (int y = 0; y < size ; y++) {
            bufferParent(x, y) = parent.getZInterpolated(
                    oX + ((double) x / size) * ratio,
                    oY + ((double) y / size) * ratio
            ) * parentProp * (unapply ? -1. : 1.);
        }
    }

    if (unapply) {
        manipulator->applyOffset(child, bufferParent);
        manipulator->multiply(child, 1. / childProp);
    }
    else {
        manipulator->multiply(child, childProp);
        manipulator->applyOffset(child, bufferParent);
    }
}
