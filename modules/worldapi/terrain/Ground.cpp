#include "Ground.h"

#include <memory>
#include <list>

#include "core/WorldTypes.h"
#include "flat/FlatWorldCollector.h"
#include "PerlinTerrainGenerator.h"
#include "ReliefMapModifier.h"
#include "TerrainOps.h"

namespace world {

    // Utility class
    class GroundContext : public ITerrainWorkerContext {
    public:
        int _x, _y, _lvl;
        int _genID;
        Ground &_ground;

        bool _registerState = false;

        GroundContext(Ground &ground, int x, int y, int lvl, int genID)
                : _x(x), _y(y), _lvl(lvl), _genID(genID), _ground(ground) {}

        optional<const Terrain &> getNeighbour(int x, int y) const override {
			if (_ground.terrainExists(x + _x, y + _y, _lvl)) {
				auto ret = _ground.cachedTerrain(x + _x, y + _y, _lvl, _genID);

				if (ret) {
				    return *ret;
				}
				else {
				    return nullopt;
				}
			}
			else {
				return nullopt;
			}
        }

        void registerCurrentState() override {
            _registerState = true;
        }
    };

    struct Tile {
        std::vector<optional<Terrain>> _cache;
        std::unique_ptr<Terrain> _final;
    };

    class PrivateGround {
    public:
        PrivateGround() : _terrains() {}

        std::map<vec3i, Tile> _terrains;
        std::list<std::unique_ptr<ITerrainWorker>> _generators;
    };

// Idees d'ameliorations :
// - Systeme de coordonnees semblable a celui du chunk system : les
// coordonnees sont relatives au niveau du dessus
// (Pour ce point, on generalisera peut-etre le system de chunk emboite
// dans une classe avec des templates)

    Ground::Ground() :
            _internal(new PrivateGround()),
            _unitSize(4000), _maxAltitude(4000), _minAltitude(-2000) {
        _internal->_generators.emplace_back(std::make_unique<PerlinTerrainGenerator>(0, 1, 4.));
        _internal->_generators.emplace_back(std::make_unique<CustomWorldRMModifier>());
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
		double xd = x / size;
		double yd = y / size;
        int xi = (int) floor(xd);
        int yi = (int) floor(yd);

        if (!terrainExists(xi, yi, lvl)) {
            generateTerrain(xi, yi, lvl);
        }

        const Terrain &terrain = const_cast<Ground *>(this)->terrain(xi, yi, lvl);
        return _minAltitude + getAltitudeRange() * terrain.getExactHeightAt(xd - xi, yd - yi);
    }

    // FIXME : Sometimes a level of details is skipped, and then problems happen
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

	// TODO optional ?
    Terrain &Ground::terrain(int x, int y, int lvl) {
        return *_internal->_terrains.at({x, y, lvl})._final;
    }

    optional<Terrain &> Ground::cachedTerrain(int x, int y, int lvl, int genID) {
        return *_internal->_terrains.at({x, y, lvl})._cache[genID];
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

        // We assume that the texture is 8 time larger... make a variable ?
        double detailRes = _terrainRes;

        // Formula obtained from the equation : getTerrainSize(lvl) / detailRes = minDetailSize
        return clamp((int) (log(_unitSize / (minDetailSize * detailRes)) / log(_factor)), 0, _maxLOD);
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
        std::unique_ptr<Terrain> terrain = std::make_unique<Terrain>(_terrainRes);

        // Bounds
        double terrainSize = getTerrainSize(lvl);
        // TODO min altitude and max altitude also depend on lvl
        terrain->setBounds(terrainSize * x, terrainSize * y, _minAltitude,
                           terrainSize * (x + 1), terrainSize * (y + 1), _maxAltitude);

        // Generation
        GroundContext context(*this, x, y, lvl, 0);
        std::vector<optional<Terrain>> cache;

        for (auto &generator : _internal->_generators) {
            generator->process(*terrain, context);

            if (context._registerState) {
                cache.emplace_back(*terrain);
                context._registerState = false;
            }
            else {
                cache.emplace_back(nullopt);
            }

            context._genID++;
        }

        _internal->_terrains[{x, y, lvl}] = {std::move(cache), std::move(terrain)};

        // Integration
        applyPreviousLayer(x, y, lvl);
    }

    void Ground::applyPreviousLayer(int x, int y, int lvl, bool unapply) {
        if (lvl != 0) {
            applyParent(x, y, lvl, unapply);
        }
    }

    void Ground::applyParent(int tX, int tY, int lvl, bool unapply) {
        vec3i childId{tX, tY, lvl};
        Terrain &child = this->terrain(tX, tY, lvl);
        int size = child.getResolution();

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

        arma::mat bufferParent(size, size);

        for (int x = 0; x < size; x++) {
            for (int y = 0; y < size; y++) {
                bufferParent(x, y) = parent.getInterpolatedHeight(
                        oX + ((double) x / (size - 1)) * ratio,
                        oY + ((double) y / (size - 1)) * ratio,
                        Interpolation::COSINE
                ) * parentProp * (unapply ? -1. : 1.);
            }
        }

        if (unapply) {
            TerrainOps::applyOffset(child, bufferParent);
            TerrainOps::multiply(child, 1. / childProp);
        } else {
            TerrainOps::multiply(child, childProp);
            TerrainOps::applyOffset(child, bufferParent);
        }
    }
}
