#include "Ground.h"

#include <memory>

#include "TerrainManipulator.h"
#include "TerrainGenerator.h"
#include "../world/FlatWorldCollector.h"

namespace world {

    // Utility class
    class GroundContext : public ITerrainGeneratorContext {
    public:
        GroundContext(Ground &ground, int x, int y, int lvl) : _x(x), _y(y), _lvl(lvl), _ground(ground) {}

        bool neighbourExists(int x, int y) const override {
            return _ground.terrainExists(x + _x, y + _y, _lvl);
        }

        const Terrain &getNeighbour(int x, int y) const override {
            return _ground.rawTerrain(x + _x, y + _y, _lvl);
        }

    private:
        int _x, _y, _lvl;
        Ground &_ground;
    };

    struct Tile {
        std::unique_ptr<Terrain> _raw;
        std::unique_ptr<Terrain> _final;
    };

    class PrivateGround {
    public:
        PrivateGround() : _terrains(), _mapUnitPerTerrain(2) {}

        /** Le nombre de pixel de map de relief par terrains de niveau 0  */
        double _mapUnitPerTerrain;
        std::map<vec2i, std::pair<std::unique_ptr<Terrain>, std::unique_ptr<Terrain>>> _map;
        std::map<vec3i, Tile> _terrains;
    };

// Idees d'ameliorations :
// - Au lieu d'apply et d'unapply la map et les terrains parents,
// conserver le terrain brut en cache aussi longtemps que nécessaire
// - Systeme de coordonnees semblable a celui du chunk system : les
// coordonnees sont relatives au niveau du dessus
// (Pour ce point, on generalisera peut-etre le system de chunk emboite
// dans une classe avec des templates)

    Ground::Ground() :
            _internal(new PrivateGround()),
            _unitSize(4000), _maxAltitude(4000), _minAltitude(-2000) {
        _terrainGenerator = std::make_unique<PerlinTerrainGenerator>(0, 1, 4);
        _mapGenerator = std::make_unique<CustomWorldRMGenerator>();
    }

    Ground::~Ground() {
        delete _internal;
    }

    double Ground::observeAltitudeAt(WorldZone zone, double x, double y) {
        int lvl = getLevelForChunk(zone);
        vec3d offset = zone->getAbsoluteOffset();
        return observeAltitudeAt(offset.x + x, offset.y + y, lvl);
    }

    void Ground::collectZone(FlatWorldCollector &collector, FlatWorld &world, const WorldZone &zone) {
        generateZone(world, zone);
        Chunk &chunk = zone->chunk();

        int lvl = getLevelForChunk(zone);

        // Find terrains to generate
        double terrainSize = lvl == 0 ? getTerrainSize(lvl) : getTerrainSize(lvl - 1);
        vec3d lower = zone->getAbsoluteOffset() / terrainSize;
        vec3d upper = lower + chunk.getSize() / terrainSize;

        for (int x = (int) floor(lower.x); x < ceil(upper.x); x++) {
            for (int y = (int) floor(lower.y); y < ceil(upper.y); y++) {

                if (lvl == 0) {
                    collector.addTerrain(getTerrainDataId(x, y, lvl), terrain(x, y, lvl));
                } else {
                    replaceTerrain(x, y, lvl - 1, collector);
                }
            }
        }
    }

    double Ground::observeAltitudeAt(double x, double y, int lvl) {
        double size = getTerrainSize(lvl);
        int xi = (int) floor(x / size);
        int yi = (int) floor(y / size);

        if (!terrainExists(xi, yi, lvl)) {
            generateTerrain(xi, yi, lvl);
        }

        const Terrain &terrain = const_cast<Ground *>(this)->terrain(xi, yi, lvl);
        return _minAltitude + getAltitudeRange() * terrain.getZInterpolated(x / size - xi, y / size - yi);
    }

    void Ground::replaceTerrain(int xp, int yp, int lvl, FlatWorldCollector &collector) {
        collector.disableTerrain(getTerrainDataId(xp, yp, lvl));

        for (int x = xp * _factor; x < xp * _factor + _factor; x++) {
            for (int y = yp * _factor; y < yp * _factor + _factor; y++) {
                if (!terrainExists(x, y, lvl + 1)) {
                    generateTerrain(x, y, lvl + 1);
                }
                collector.addTerrain(getTerrainDataId(x, y, lvl + 1), terrain(x, y, lvl + 1));
            }
        }
    }

    Terrain &Ground::terrain(int x, int y, int lvl) {
        return *_internal->_terrains.at({x, y, lvl})._final;
    }

    Terrain &Ground::rawTerrain(int x, int y, int lvl) {
        return *_internal->_terrains.at({x, y, lvl})._raw;
    }

    bool Ground::terrainExists(int x, int y, int lvl) const {
        return _internal->_terrains.find({x, y, lvl}) != _internal->_terrains.end();
    }

    std::string Ground::getTerrainDataId(int x, int y, int lvl) const {
        u64 id = (u64) (x & 0x0FFFFFFF)
                      + ((u64) (y & 0x0FFFFFFF) << 24)
                      + ((u64) (lvl & 0xFF) << 48);
        return std::to_string(id);
    }


    double Ground::getTerrainSize(int lvl) const {
        return _unitSize * powi((double) _factor, -lvl);
    }

    double Ground::getLayerContribution(int level) const {
        return level == 0 ? 1 : 0.1 * powi(0.5, level - 1);
    }

    int Ground::getLevelForChunk(const WorldZone &zone) const {
        double minDetailSize = zone->getChunk().getMinDetailSize();

        // Avoid division by zero
        if (minDetailSize < std::numeric_limits<double>::epsilon()) {
            return _maxLOD;
        }

        // Formula obtained from the equation : getTerrainSize(lvl) / terrainRes = minDetailSize
        return clamp((int) (log(_unitSize / (minDetailSize * _terrainRes)) / log(_factor)), 0, _maxLOD);
    }

    vec3i Ground::getParentId(const vec3i &childId) const {
        return {
                (childId.x - mod(childId.x, _factor)) / _factor,
                (childId.y - mod(childId.y, _factor)) / _factor,
                childId.z - 1
        };
    }

    void Ground::generateZone(FlatWorld &world, const WorldZone &zone) {
        Chunk &chunk = zone->chunk();

        // Calculate lvl for the given chunk
        int lvl = getLevelForChunk(zone);

        // Find terrains to generate
        double terrainSize = getTerrainSize(lvl);
        vec3d lower = chunk.getOffset() / terrainSize;
        vec3d upper = lower + chunk.getSize() / terrainSize;

        for (int x = (int) floor(lower.x); x < ceil(upper.x); x++) {
            for (int y = (int) floor(lower.y); y < ceil(upper.y); y++) {
                if (!terrainExists(x, y, lvl)) {
                    generateTerrain(x, y, lvl);
                }
            }
        }
    }

    void Ground::generateTerrain(int x, int y, int lvl) {
        // Terrain creation
        std::unique_ptr<Terrain> raw = std::make_unique<Terrain>(_terrainRes);
        _terrainGenerator->process(*raw, GroundContext(*this, x, y, lvl));

        std::unique_ptr<Terrain> final = std::make_unique<Terrain>(*raw);

        // Bounds
        double terrainSize = getTerrainSize(lvl);
        // TODO min altitude and max altitude also depend on lvl
        final->setBounds(terrainSize * x, terrainSize * y, _minAltitude,
                           terrainSize * (x + 1), terrainSize * (y + 1), _maxAltitude);

        _internal->_terrains[{x, y, lvl}] = {std::move(raw), std::move(final)};

        // Integration
        applyPreviousLayer(x, y, lvl);
    }

    void Ground::applyPreviousLayer(int x, int y, int lvl, bool unapply) {
        if (lvl == 0) {
            applyMap(x, y, lvl, unapply);
        } else {
            if (!unapply) {
                applyMap(x, y, lvl, unapply);
            }
            applyParent(x, y, lvl, unapply);
            if (unapply) {
                applyMap(x, y, lvl, unapply);
            }
        }
    }

    void Ground::applyParent(int tX, int tY, int lvl, bool unapply) {
        vec3i childId{tX, tY, lvl};
        Terrain &child = this->terrain(tX, tY, lvl);
        int size = child.getSize();

        // Get parent. Generate it if needed
        auto parentId = getParentId(childId);

        if (!terrainExists(parentId.x, parentId.y, parentId.z)) {
            generateTerrain(parentId.x, parentId.y, parentId.z);
        }

        Terrain &parent = this->terrain(parentId.x, parentId.y, parentId.z);

        // Useful variables
        const double parentProp = 1;
        const double childProp = getLayerContribution(lvl);
        const double oX = (double) mod(tX, _factor) / _factor;
        const double oY = (double) mod(tY, _factor) / _factor;
        const double ratio = 1. / _factor;

        std::unique_ptr<ITerrainManipulator> manipulator(ITerrainManipulator::createManipulator());

        arma::mat bufferParent(size, size);

        for (int x = 0; x < size; x++) {
            for (int y = 0; y < size; y++) {
                bufferParent(x, y) = parent.getZInterpolated(
                        oX + ((double) x / (size - 1)) * ratio,
                        oY + ((double) y / (size - 1)) * ratio
                ) * parentProp * (unapply ? -1. : 1.);
            }
        }

        if (unapply) {
            manipulator->applyOffset(child, bufferParent);
            manipulator->multiply(child, 1. / childProp);
        } else {
            manipulator->multiply(child, childProp);
            manipulator->applyOffset(child, bufferParent);
        }
    }

    void Ground::applyMap(int tX, int tY, int lvl, bool unapply) {

        // Terrain
        Terrain &terrain = this->terrain(tX, tY, lvl);
        int size = terrain.getSize();

        // Map
        Terrain &heightMap = getOrCreateOffsetMap(0, 0);
        Terrain &diffMap = getOrCreateDiffMap(0, 0);

        // Setup variables
        const double terrainRelativeSize = getTerrainSize(lvl) / getTerrainSize(0);
        const double ratio = _internal->_mapUnitPerTerrain * terrainRelativeSize / heightMap.getSize();
        const double mapOx = 0.5 + tX * ratio;
        const double mapOy = 0.5 + tY * ratio;

        const double offsetCoef = 0.5;
        const double diffCoef = 1 - offsetCoef;

        //std::cout << (unapply ? "unapply " : "apply ") << "to" << tX << ", " << tY << std::endl;

        std::unique_ptr<ITerrainManipulator> manipulator(ITerrainManipulator::createManipulator());

        arma::mat bufferOffset(size, size);
        arma::mat bufferDiff(size, size);

        for (int x = 0; x < size; x++) {
            for (int y = 0; y < size; y++) {
                double mapX = mapOx + ((double) x / (size - 1)) * ratio;
                double mapY = mapOy + ((double) y / (size - 1)) * ratio;

                double offset = offsetCoef * heightMap.getZInterpolated(mapX, mapY);
                double diff = diffMap.getZInterpolated(mapX, mapY) * diffCoef;

                if (unapply) {
                    bufferOffset(x, y) = -offset;
                    bufferDiff(x, y) = diff > std::numeric_limits<double>::epsilon() ? 1 / diff : 0;
                } else {
                    bufferOffset(x, y) = offset;
                    bufferDiff(x, y) = diff;
                }
            }
        }

        if (unapply) {
            manipulator->applyOffset(terrain, bufferOffset);
            manipulator->multiply(terrain, bufferDiff);
        } else {
            manipulator->multiply(terrain, bufferDiff);
            manipulator->applyOffset(terrain, bufferOffset);
        }
    }

    std::pair<std::unique_ptr<Terrain>, std::unique_ptr<Terrain>>& Ground::getOrCreateMap(int x, int y) {
        vec2i mapLoc(x, y);
        auto it = _internal->_map.find(mapLoc);

        if (it == _internal->_map.end()) {
            it = _internal->_map.emplace(mapLoc, std::make_pair(
                    std::make_unique<Terrain>(100), std::make_unique<Terrain>(100)
            )).first;
            _mapGenerator->generate(*it->second.first, *it->second.second);
        }

        return it->second;
    }

    Terrain& Ground::getOrCreateOffsetMap(int x, int y) {
        return *getOrCreateMap(x, y).first;
    }

    Terrain& Ground::getOrCreateDiffMap(int x, int y) {
        return *getOrCreateMap(x, y).second;
    }
}
