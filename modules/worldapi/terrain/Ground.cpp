#include "Ground.h"

#include <memory>
#include <list>

#include "core/WorldTypes.h"
#include "math/MathsHelper.h"
#include "flat/FlatWorldCollector.h"
#include "ApplyParentTerrain.h"
#include "PerlinTerrainGenerator.h"
#include "ReliefMapModifier.h"
#include "AltitudeTexturer.h"
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

		optional<const Terrain &> getParent() const override {
			if (_lvl == 0)
				return nullopt;

			auto parentId = _ground.getParentId({ _x, _y, _lvl });

			if (!_ground.terrainExists(parentId.x, parentId.y, parentId.z)) {
			    _ground.generateTerrain(parentId.x, parentId.y, parentId.z);
			}

			auto ret = _ground.cachedTerrain(parentId.x, parentId.y, parentId.z, _genID);

			if (ret) {
				return *ret;
			}
			else {
				return nullopt;
			}
		}

		int getParentCount() const override {
			return _lvl;
		}

        void registerCurrentState() override {
            _registerState = true;
        }
    };

    struct Tile {
        std::vector<optional<Terrain>> _cache;
        std::unique_ptr<Terrain> _final;
        std::unique_ptr<Mesh> _mesh;
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

    Ground::Ground(double unitSize, double minAltitude, double maxAltitude) :
            _internal(new PrivateGround()),
            _unitSize(unitSize), _minAltitude(minAltitude), _maxAltitude(maxAltitude) {
        _internal->_generators.emplace_back(std::make_unique<PerlinTerrainGenerator>(0, 1, 4.));
        _internal->_generators.emplace_back(std::make_unique<CustomWorldRMModifier>());
        _internal->_generators.emplace_back(std::make_unique<ApplyParentTerrain>());

        // Texturer
        auto texturer = std::make_unique<AltitudeTexturer>(_textureRes);
        ColorMap &colorMap = texturer->getColorMap();

        colorMap.addPoint({ 0, 0}, Color4u(209, 207, 153));
        colorMap.addPoint({ 0, 1}, Color4u(209, 207, 153));
        colorMap.addPoint({ 0.37, 0 }, Color4u(133, 183, 144));
        colorMap.addPoint({ 0.3, 1 }, Color4u(144, 183, 123));
        colorMap.addPoint({ 0.48, 1 }, Color4u(96, 76, 40));
        colorMap.addPoint({ 0.55, 0 }, Color4u(114, 90, 48));
        colorMap.addPoint({ 0.65, 1 }, Color4u(160, 160, 160));
        colorMap.addPoint({ 0.8, 0.5 }, Color4u(160, 160, 160));
        colorMap.addPoint({ 1, 0 }, Color4u(244, 252, 250));
        colorMap.addPoint({ 1, 1 }, Color4u(244, 252, 250));

        _internal->_generators.emplace_back(std::move(texturer));
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
        Chunk &chunk = zone->chunk();
        vec3d offset = zone->getAbsoluteOffset();
        vec3d chunkSize = chunk.getSize();

        // skip if zone is not in altitude range
        double estimAltitude = observeAltitudeAt(offset.x + chunkSize.x / 2, offset.y + chunkSize.y / 2, 0);

        if (abs(estimAltitude - offset.z) >= chunkSize.z / 2
            && abs(estimAltitude - offset.z - chunkSize.z) >= chunkSize.z / 2) {

            return;
        }

        // Generate the zone if needed
        generateZone(world, zone);

        // Find terrains to generate
        int lvl = getLevelForChunk(zone);
        double terrainSize = lvl == 0 ? getTerrainSize(lvl) : getTerrainSize(lvl - 1);
        vec3d lower = offset / terrainSize;
        vec3d upper = lower + chunkSize / terrainSize;

        for (int x = (int) floor(lower.x); x < ceil(upper.x); x++) {
            for (int y = (int) floor(lower.y); y < ceil(upper.y); y++) {

                if (lvl == 0) {
                    addTerrain(x, y, lvl, collector);
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

        const Terrain &terrain = this->terrain(xi, yi, lvl);
        return _minAltitude + getAltitudeRange() * terrain.getExactHeightAt(xd - xi, yd - yi);
    }

    inline ICollector::ItemKey terrainToItem(const std::string &key) {
        return ICollector::ItemKeys::inWorld(std::string("_") + key, ObjectKeys::defaultKey(), 0);
    }

    // FIXME : Sometimes a level of details is skipped, and then problems happen
    void Ground::replaceTerrain(int xp, int yp, int lvl, FlatWorldCollector &collector) {
        collector.disableItem(terrainToItem(getTerrainDataId(xp, yp, lvl)));

        for (int x = xp * _factor; x < xp * _factor + _factor; x++) {
            for (int y = yp * _factor; y < yp * _factor + _factor; y++) {
                if (!terrainExists(x, y, lvl + 1)) {
                    generateTerrain(x, y, lvl + 1);
                }
                addTerrain(x, y, lvl + 1, collector);
            }
        }
    }

    void Ground::addTerrain(int x, int y, int lvl, ICollector &collector) {
        ICollector::ItemKey itemKey = terrainToItem(getTerrainDataId(x, y, lvl));
        Terrain &terrain = this->terrain(x, y, lvl);

        if (!collector.hasItem(itemKey)) {
            // Relocate the terrain
            auto & bbox = terrain.getBoundingBox();
            vec3d offset = bbox.getLowerBound();
            vec3d size = bbox.getUpperBound() - offset;

            // Create the mesh
            auto meshbe = mesh(x, y, lvl);
            Object3D object;

            if (meshbe) {
                object.setMesh(*meshbe, true);
            }
            else {
                std::unique_ptr<Mesh> &mesh =
                        (_internal->_terrains[{x, y, lvl}]._mesh =
                                 std::unique_ptr<Mesh>(terrain.createMesh(0, 0, 0, size.x, size.y, size.z)));
                object.setMesh(*mesh, true);
            }
            object.setPosition(offset);
            object.setMaterialID("terrain");

            // Create the material
            Material material("terrain");
            material.setKd(1, 1, 1);
            //material.setKd(1, (double) lvl / _maxLOD, 1 - ((double) lvl / _maxLOD));
            material.setMapKd("texture01");

            // Retrieve the texture
            auto texture = terrain.getTexture();

            collector.addItem(itemKey, object);
            collector.addMaterial(itemKey, material);

            if (texture) {
                collector.addTexture(itemKey, "texture01", *texture, true);
            }
        }
    }

	// TODO optional ?
    Terrain &Ground::terrain(int x, int y, int lvl) {
        return *_internal->_terrains.at({x, y, lvl})._final;
    }

    optional<Mesh &> Ground::mesh(int x, int y, int lvl) {
        auto &meshPtr = _internal->_terrains.at({x, y, lvl})._mesh;
        if (meshPtr) {
            return *meshPtr;
        }
        else {
            return nullopt;
        }
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

    double Ground::getTerrainResolution(int lvl) const {
        // * 2 -> we don't need to be pixel precise with terrains, the details will be added later
        return _terrainRes * _textureRes * 2 / getTerrainSize(lvl);
    }

    int Ground::getLevelForChunk(const WorldZone &zone) const {
        double chunkMaxRes = zone->getChunk().getMaxResolution();

        for (int lvl = 0; lvl < _maxLOD; lvl++) {
            if (getTerrainResolution(lvl + 1) > chunkMaxRes)
                return lvl;
        }

        return _maxLOD;
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
    }
}
