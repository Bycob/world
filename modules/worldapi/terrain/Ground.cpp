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
class GroundContext : public ITileContext {
public:
    TerrainKey _key;
    int _genID;
    Ground &_ground;

    bool _registerState = false;

    GroundContext(Ground &ground, const TerrainKey &key, int genID)
            : _key(key), _genID(genID), _ground(ground) {}

    Terrain &getTerrain() const override {
        return _ground.provideTerrain(_key);
    }

    Image &getTexture() const override {
        return _ground.provideTerrain(_key).getTexture();
    }

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

    colorMap.addPoint({0.15, 0.5}, Color4u(209, 207, 153)); // Sand
    colorMap.addPoint({0.31, 0}, Color4u(209, 207, 153)); // Sand
    colorMap.addPoint({0.31, 1}, Color4u(209, 207, 153)); // Sand
    colorMap.addPoint({0.35, 0}, Color4u(72, 132, 85)); // Grass blueish
    colorMap.addPoint({0.35, 1}, Color4u(144, 183, 123)); // Grass yellowish
    colorMap.addPoint({0.5, 0}, Color4u(114, 90, 48)); // Light dirt
    colorMap.addPoint({0.53, 1}, Color4u(96, 76, 40)); // Dark dirt
    colorMap.addPoint({0.65, 0}, Color4u(160, 160, 160)); // Rock
    colorMap.addPoint({0.8, 1}, Color4u(160, 160, 160)); // Rock
    colorMap.addPoint({1, 0}, Color4u(244, 252, 250)); // Snow
    colorMap.addPoint({1, 1}, Color4u(244, 252, 250)); // Snow
}

double Ground::observeAltitudeAt(WorldZone zone, double x, double y) {
    int lvl = getLevelForChunk(zone);
    vec3d offset = zone->getAbsoluteOffset();
    return observeAltitudeAt(offset.x + x, offset.y + y, lvl);
}

void Ground::collectZone(const WorldZone &zone, ICollector &collector) {
    vec3d offset = zone->getAbsoluteOffset();
    vec3d chunkSize = zone->getDimensions();

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

        // Create the mesh
        Object3D object(provideMesh(key));
        object.setPosition(offset);
        object.setMaterialID("terrain");

        // Create the material
        Material material("terrain");
        material.setKd(1, 1, 1);
        // material.setKd(1, (double) lvl / _maxLOD, 1 - ((double) lvl /
        // _maxLOD));
        material.setMapKd("texture01");

        // Retrieve the texture
        auto &texture = terrain.getTexture();

        collector.addItem(itemKey, object);
        collector.addMaterial(itemKey, material);
        collector.addTexture(itemKey, "texture01", texture);
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

Mesh &Ground::provideMesh(const TerrainKey &key) {
    auto &meshPtr = provide(key)._mesh;

    if (!meshPtr) {
        generateMesh(key);
    }

    return *meshPtr;
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
    u64 id = static_cast<u64>(key.pos.x & 0x0FFFFFFFu) +
             (static_cast<u64>(key.pos.y & 0x0FFFFFFFu) << 24u) +
             (static_cast<u64>(key.lod & 0xFFu) << 48u);
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
    double chunkMaxRes = zone->getMaxResolution();

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

    // Tile creation
    Tile &tile = _internal->_terrains[key] = Tile{
        std::vector<optional<Terrain>>(),
        std::make_unique<Terrain>(_terrainRes), std::unique_ptr<Mesh>(), 0};

    Terrain &terrain = *tile._final;
    std::vector<optional<Terrain>> &cache = _internal->_terrains[key]._cache;

    // Parameters
    auto texSize = _terrainRes * _textureRes;
    terrain.setTexture(Image(texSize, texSize, ImageType::RGB));

    double terrainSize = getTerrainSize(key.lod);
    terrain.setBounds(terrainSize * key.pos.x, terrainSize * key.pos.y,
                      _minAltitude, terrainSize * (key.pos.x + 1),
                      terrainSize * (key.pos.y + 1), _maxAltitude);

    // Generation
    GroundContext context(*this, key, 0);

    for (auto &generator : _internal->_generators) {
        generator->processTile(context);

        if (context._registerState) {
            cache.emplace_back(terrain);
            context._registerState = false;
        } else {
            cache.emplace_back(nullopt);
        }

        context._genID++;
    }
}

void Ground::generateMesh(const TerrainKey &key) {
    // Find required terrains
    Terrain *terrains[3][3];

    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            TerrainKey jitteredKey = key;
            jitteredKey.pos.x += i - 1;
            jitteredKey.pos.y += j - 1;
            terrains[i][j] = &provideTerrain(jitteredKey);
        }
    }

    // Fill mesh
    // Same as Terrain::createMesh, but may become different
    // + here we compute normals a different way (for tiling to be acceptable).
    auto &meshPtr = provide(key)._mesh;
    meshPtr = std::make_unique<Mesh>();
    Mesh &mesh = *meshPtr;

    // TODO compute size from TileSystem
    BoundingBox bbox = terrains[1][1]->getBoundingBox();
    const double offsetX = 0, offsetY = 0, offsetZ = 0;
    const double sizeX = bbox.getDimensions().x;
    const double sizeY = bbox.getDimensions().y;
    const double sizeZ = bbox.getDimensions().z;

    const int size = terrains[1][1]->getResolution();
    const int size_1 = size - 1;
    const double inv_size_1 = 1. / size_1;

    auto valueAt = [&terrains, size](int x, int y) -> double {
        // xl = x_local, xg = x_global
        int xl = mod(x, size);
        int yl = mod(y, size);
        int xg = 1 + (x - xl) / size;
        int yg = 1 + (y - yl) / size;
        return terrains[xg][yg]->operator()(xl + xg - 1, yl + yg - 1);
    };

    // Memory allocation
    int vertCount = size * size;
    mesh.reserveVertices(vertCount);

    // Vertices
    for (int y = 0; y < size; y++) {
        const double yd = y * inv_size_1;
        const double ypos = yd * sizeY + offsetY;

        for (int x = 0; x < size; x++) {
            const double xd = x * inv_size_1;
            const double xpos = xd * sizeX + offsetX;

            Vertex &vert = mesh.newVertex();

            vert.setPosition(xpos, ypos, valueAt(x, y) * sizeZ + offsetZ);
            vert.setTexture(xd, 1 - yd);
            if (vert.getTexture().y > 1 || vert.getTexture().y < 0)
                std::cout << vert.getTexture().y << std::endl;

            // Compute normal
            double xUnit = sizeX * inv_size_1;
            double yUnit = sizeY * inv_size_1;
            vec3d nx{(valueAt(x - 1, y) - valueAt(x + 1, y)) * sizeZ, 0,
                     xUnit * 2};
            vec3d ny{0, (valueAt(x, y - 1) - valueAt(x, y + 1)) * sizeZ,
                     yUnit * 2};
            vert.setNormal((nx + ny).normalize());
        }
    }

    // Faces
    auto indice = [size](int x, int y) -> int { return y * size + x; };
    mesh.reserveFaces(size_1 * size_1 * 2);

    for (int y = 0; y < size_1; y++) {
        for (int x = 0; x < size_1; x++) {
            Face &face1 = mesh.newFace();
            Face &face2 = mesh.newFace();

            face1.setID(0, indice(x, y));
            face1.setID(1, indice(x + 1, y));
            face1.setID(2, indice(x, y + 1));

            face2.setID(0, indice(x + 1, y + 1));
            face2.setID(1, indice(x, y + 1));
            face2.setID(2, indice(x + 1, y));
        }
    }
}

} // namespace world
