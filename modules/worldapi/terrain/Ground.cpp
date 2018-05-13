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

struct Ground::TerrainKey {
    vec2i pos;
    int lod;

    TerrainKey(int x, int y, int lod) : pos{x, y}, lod(lod) {}
};

struct Ground::Tile {
    std::vector<optional<Terrain>> _cache;
    std::unique_ptr<Terrain> _final;
    std::unique_ptr<Mesh> _mesh;
    u64 _lastAccess;
};

using TerrainKey = Ground::TerrainKey;

using Tile = Ground::Tile;

bool operator<(const TerrainKey &key1, const TerrainKey &key2) {
    return key1.lod < key2.lod ? true
                               : (key1.lod == key2.lod && key1.pos < key2.pos);
}

// Utility class
class GroundContext : public ITerrainWorkerContext {
public:
    TerrainKey _key;
    int _genID;
    Ground &_ground;

    bool _registerState = false;

    GroundContext(Ground &ground, const TerrainKey &key, int genID)
            : _key(key), _genID(genID), _ground(ground) {}

    optional<const Terrain &> getNeighbour(int x, int y) const override {
        TerrainKey nKey{_key.pos.x + x, _key.pos.y + y, _key.lod};
        auto ret = _ground.getCachedTerrain(nKey, _genID);

        if (ret) {
            return *ret;
        } else {
            return nullopt;
        }
    }

    optional<const Terrain &> getParent() const override {
        if (_key.lod == 0)
            return nullopt;

        auto parentId = _ground.getParentKey(_key);
        auto ret = _ground.getCachedTerrain(parentId, _genID);

        if (ret) {
            return *ret;
        } else {
            return nullopt;
        }
    }

    int getParentCount() const override { return _key.lod; }

    vec2i getTileCoords() const override { return _key.pos; }

    void registerCurrentState() override { _registerState = true; }
};

class PGround {
public:
    PGround() : _terrains() {}

    std::map<TerrainKey, Tile> _terrains;
    std::list<std::unique_ptr<ITerrainWorker>> _generators;

    u64 _accessCounter = 0;
    std::map<u64, TerrainKey> _accesses;
};

// Idees d'ameliorations :
// - Systeme de coordonnees semblable a celui du chunk system : les
// coordonnees sont relatives au niveau du dessus
// (Pour ce point, on generalisera peut-etre le system de chunk emboite
// dans une classe avec des templates)

Ground::Ground(double unitSize, double minAltitude, double maxAltitude)
        : _internal(new PGround()), _unitSize(unitSize),
          _minAltitude(minAltitude), _maxAltitude(maxAltitude) {}

Ground::~Ground() { delete _internal; }

void Ground::setDefaultWorkerSet() {
    addWorker<PerlinTerrainGenerator>(3, 4., 0.35);
    addWorker<CustomWorldRMModifier>();

    // Texturer
    auto &texturer = addWorker<AltitudeTexturer>(_textureRes);
    ColorMap &colorMap = texturer.getColorMap();

    colorMap.addPoint({0, 0}, Color4u(209, 207, 153));
    colorMap.addPoint({0, 1}, Color4u(209, 207, 153));
    colorMap.addPoint({0.37, 0}, Color4u(133, 183, 144));
    colorMap.addPoint({0.3, 1}, Color4u(144, 183, 123));
    colorMap.addPoint({0.48, 1}, Color4u(96, 76, 40));
    colorMap.addPoint({0.55, 0}, Color4u(114, 90, 48));
    colorMap.addPoint({0.65, 1}, Color4u(160, 160, 160));
    colorMap.addPoint({0.8, 0.5}, Color4u(160, 160, 160));
    colorMap.addPoint({1, 0}, Color4u(244, 252, 250));
    colorMap.addPoint({1, 1}, Color4u(244, 252, 250));
}

double Ground::observeAltitudeAt(WorldZone zone, double x, double y) {
    int lvl = getLevelForChunk(zone);
    vec3d offset = zone->getAbsoluteOffset();
    return observeAltitudeAt(offset.x + x, offset.y + y, lvl);
}

void Ground::collectZone(const WorldZone &zone, ICollector &collector) {
    Chunk &chunk = zone->chunk();
    vec3d offset = zone->getAbsoluteOffset();
    vec3d chunkSize = chunk.getSize();

    // skip if zone is not in altitude range
    double estimAltitude = observeAltitudeAt(offset.x + chunkSize.x / 2,
                                             offset.y + chunkSize.y / 2, 0);

    if (abs(estimAltitude - offset.z) >= chunkSize.z / 2 &&
        abs(estimAltitude - offset.z - chunkSize.z) >= chunkSize.z / 2) {

        return;
    }

    // Find terrains to generate
    int lvl = getLevelForChunk(zone);
    double terrainSize =
        lvl == 0 ? getTerrainSize(lvl) : getTerrainSize(lvl - 1);
    vec3d lower = offset / terrainSize;
    vec3d upper = lower + chunkSize / terrainSize;

    for (int x = (int)floor(lower.x); x < ceil(upper.x); x++) {
        for (int y = (int)floor(lower.y); y < ceil(upper.y); y++) {

            if (lvl == 0) {
                addTerrain({x, y, lvl}, collector);
            } else {
                replaceTerrain({x, y, lvl - 1}, collector);
            }
        }
    }

    updateCache();
}

void Ground::collectZone(const WorldZone &zone, FlatWorld &world,
                         FlatWorldCollector &collector) {
    collectZone(zone, collector);
}

void Ground::addWorkerInternal(ITerrainWorker *worker) {
    _internal->_generators.push_back(std::unique_ptr<ITerrainWorker>(worker));
}

double Ground::observeAltitudeAt(double x, double y, int lvl) {
    double size = getTerrainSize(lvl);
    double xd = x / size;
    double yd = y / size;
    int xi = (int)floor(xd);
    int yi = (int)floor(yd);
    TerrainKey key(xi, yi, lvl);

    const Terrain &terrain = this->provideTerrain(key);
    updateCache();
    return _minAltitude +
           getAltitudeRange() * terrain.getExactHeightAt(xd - xi, yd - yi);
}

inline ICollector::ItemKey terrainToItem(const std::string &key) {
    return ICollector::ItemKeys::inWorld(std::string("_") + key,
                                         ObjectKeys::defaultKey(), 0);
}

// FIXME : Sometimes a level of details is skipped, and then problems happen
void Ground::replaceTerrain(const TerrainKey &pKey, ICollector &collector) {
    collector.disableItem(terrainToItem(getTerrainDataId(pKey)));
    auto cp = pKey.pos;

    for (int x = cp.x * _factor; x < cp.x * _factor + _factor; x++) {
        for (int y = cp.y * _factor; y < cp.y * _factor + _factor; y++) {
            TerrainKey cKey(x, y, pKey.lod + 1);
            addTerrain(cKey, collector);
        }
    }
}

void Ground::addTerrain(const TerrainKey &key, ICollector &collector) {
    ICollector::ItemKey itemKey = terrainToItem(getTerrainDataId(key));
    Terrain &terrain = this->provideTerrain(key);

    if (!collector.hasItem(itemKey)) {
        // Relocate the terrain
        auto &bbox = terrain.getBoundingBox();
        vec3d offset = bbox.getLowerBound();
        vec3d size = bbox.getUpperBound() - offset;

        // Create the mesh
        auto meshbe = provideMesh(key);
        Object3D object;

        if (meshbe) {
            object.setMesh(*meshbe);
        } else {
            std::unique_ptr<Mesh> &mesh =
                (_internal->_terrains[key]._mesh = std::unique_ptr<Mesh>(
                     terrain.createMesh(0, 0, 0, size.x, size.y, size.z)));
            object.setMesh(*mesh);
        }
        object.setPosition(offset);
        object.setMaterialID("terrain");

        // Create the material
        Material material("terrain");
        material.setKd(1, 1, 1);
        // material.setKd(1, (double) lvl / _maxLOD, 1 - ((double) lvl /
        // _maxLOD));
        material.setMapKd("texture01");

        // Retrieve the texture
        auto texture = terrain.getTexture();

        collector.addItem(itemKey, object);
        collector.addMaterial(itemKey, material);

        if (texture) {
            collector.addTexture(itemKey, "texture01", *texture);
        }
    }
}

void Ground::updateCache() {
    if (_internal->_terrains.size() > _maxCacheSize) {

        // We shrink one third of the memory
        auto count = _internal->_terrains.size() / 3;

        while (count != 0) {
            auto accessEntry = _internal->_accesses.begin();

            // Free memory
            // TODO call to a drop manager
            _internal->_terrains.erase(accessEntry->second);

            // Remove access entry
            _internal->_accesses.erase(accessEntry);

            count--;
        }
        std::cout << "Dropped memory : " << _internal->_terrains.size()
                  << " left." << std::endl;
    }
}

Tile &Ground::provide(const TerrainKey &key) {
    auto it = _internal->_terrains.find(key);

    if (it == _internal->_terrains.end()) {
        generateTerrain(key);
        it = _internal->_terrains.find(key);
    }

    registerAccess(key, it->second);
    return it->second;
}

void Ground::registerAccess(const TerrainKey &key, Tile &tile) {
    _internal->_accessCounter++;

    // Remove last access entry for this tile
    if (tile._lastAccess != 0) {
        _internal->_accesses.erase(tile._lastAccess);
    }

    // Make a new entry
    tile._lastAccess = _internal->_accessCounter;
    _internal->_accesses.emplace(tile._lastAccess, key);

    // Update parent
    if (key.lod != 0) {
        auto parentKey = getParentKey(key);
        registerAccess(parentKey, provide(parentKey));
    }
}

Terrain &Ground::provideTerrain(const TerrainKey &key) {
    return *provide(key)._final;
}

optional<Mesh &> Ground::provideMesh(const TerrainKey &key) {
    auto &meshPtr = provide(key)._mesh;
    if (meshPtr) {
        return *meshPtr;
    } else {
        return nullopt;
    }
}

optional<Terrain &> Ground::getCachedTerrain(const TerrainKey &key, int genID) {
    auto it = _internal->_terrains.find(key);

    if (it == _internal->_terrains.end()) {
        return nullopt;
    } else {
        registerAccess(key, it->second);
        return *it->second._cache[genID];
    }
}

std::string Ground::getTerrainDataId(const TerrainKey &key) const {
    u64 id = (u64)(key.pos.x & 0x0FFFFFFF) +
             ((u64)(key.pos.y & 0x0FFFFFFF) << 24) +
             ((u64)(key.lod & 0xFF) << 48);
    return std::to_string(id);
}

double Ground::getTerrainSize(int lvl) const {
    return _unitSize * powi((double)_factor, -lvl);
}

double Ground::getTerrainResolution(int lvl) const {
    // * 2 -> we don't need to be pixel precise with terrains, the details will
    // be added later
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

TerrainKey Ground::getParentKey(const TerrainKey &childId) const {
    return {(childId.pos.x - mod(childId.pos.x, _factor)) / _factor,
            (childId.pos.y - mod(childId.pos.y, _factor)) / _factor,
            childId.lod - 1};
}

void Ground::generateTerrain(const TerrainKey &key) {
    // Ensure that parent was created
    if (key.lod != 0) {
        provide(getParentKey(key));
    }

    // Terrain creation
    std::unique_ptr<Terrain> terrain = std::make_unique<Terrain>(_terrainRes);

    // Bounds
    double terrainSize = getTerrainSize(key.lod);
    // TODO min altitude and max altitude also depend on lvl
    terrain->setBounds(terrainSize * key.pos.x, terrainSize * key.pos.y,
                       _minAltitude, terrainSize * (key.pos.x + 1),
                       terrainSize * (key.pos.y + 1), _maxAltitude);

    // Generation
    GroundContext context(*this, key, 0);
    std::vector<optional<Terrain>> cache;

    for (auto &generator : _internal->_generators) {
        generator->process(*terrain, context);

        if (context._registerState) {
            cache.emplace_back(*terrain);
            context._registerState = false;
        } else {
            cache.emplace_back(nullopt);
        }

        context._genID++;
    }

    _internal->_terrains[key] =
        Tile{std::move(cache), std::move(terrain), std::unique_ptr<Mesh>(), 0};
}
} // namespace world
