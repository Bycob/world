#include "Ground.h"

#include <memory>
#include <list>

#include "core/WorldTypes.h"
#include "core/ResolutionModelContextWrap.h"
#include "math/MathsHelper.h"
#include "flat/FlatWorldCollector.h"
#include "ApplyParentTerrain.h"
#include "PerlinTerrainGenerator.h"
#include "ReliefMapModifier.h"
#include "AltitudeTexturer.h"
#include "TerrainOps.h"

namespace world {

struct Ground::Tile {
    std::vector<optional<Terrain>> _cache;
    std::unique_ptr<Terrain> _final;
    std::unique_ptr<Mesh> _mesh;
    u64 _lastAccess;
};

using Tile = Ground::Tile;

// Utility class
class GroundContext : public ITileContext {
public:
    TileCoordinates _key;
    int _genID;
    Ground &_ground;

    bool _registerState = false;

    GroundContext(Ground &ground, const TileCoordinates &key, int genID)
            : _key(key), _genID(genID), _ground(ground) {}

    Terrain &getTerrain() const override {
        return _ground.provideTerrain(_key);
    }

    Image &getTexture() const override {
        return _ground.provideTerrain(_key).getTexture();
    }

    optional<const Terrain &> getNeighbour(int x, int y) const override {
        TileCoordinates nKey{_key._pos.x + x, _key._pos.y + y, 0, _key._lod};
        auto ret = _ground.getCachedTerrain(nKey, _genID);

        if (ret) {
            return *ret;
        } else {
            return nullopt;
        }
    }

    optional<const Terrain &> getParent() const override {
        if (_key._lod == 0)
            return nullopt;

        auto parentId = _ground._tileSystem.getParentTileCoordinates(_key);
        auto ret = _ground.getCachedTerrain(parentId, _genID);

        if (ret) {
            return *ret;
        } else {
            return nullopt;
        }
    }

    int getParentCount() const override { return _key._lod; }

    vec2i getTileCoords() const override { return static_cast<vec2i>(_key._pos); }

    void registerCurrentState() override { _registerState = true; }
};

class PGround {
public:
    PGround() : _terrains() {}

    std::map<TileCoordinates, Tile> _terrains;
    std::list<std::unique_ptr<ITerrainWorker>> _generators;

    u64 _accessCounter = 0;
    std::map<u64, TileCoordinates> _accesses;
};

// Idees d'ameliorations :
// - Systeme de coordonnees semblable a celui du chunk system : les
// coordonnees sont relatives au niveau du dessus
// (Pour ce point, on generalisera peut-etre le system de chunk emboite
// dans une classe avec des templates)

Ground::Ground(double unitSize, double minAltitude, double maxAltitude)
        : _internal(new PGround()),
		_minAltitude(minAltitude), _maxAltitude(maxAltitude),
		_tileSystem(5, vec3d(_terrainRes * _textureRes * 4, _terrainRes * _textureRes * 4, 0), vec3d(unitSize, unitSize, 0)) {}

Ground::~Ground() { delete _internal; }

void Ground::setDefaultWorkerSet() {
    addWorker<PerlinTerrainGenerator>(3, 4., 0.35);
    addWorker<CustomWorldRMModifier>();

    // Texturer
    auto &texturer = addWorker<AltitudeTexturer>(_textureRes);
    ColorMap &colorMap = texturer.getColorMap();

    colorMap.addPoint({0.15, 0.5}, Color4u(209, 207, 153)); // Sand
    colorMap.addPoint({0.31, 0}, Color4u(209, 207, 153));   // Sand
    colorMap.addPoint({0.31, 1}, Color4u(209, 207, 153));   // Sand
    colorMap.addPoint({0.35, 0}, Color4u(144, 183, 92));    // Light grass
    colorMap.addPoint({0.35, 1}, Color4u(72, 132, 65));     // Dark grass
    colorMap.addPoint({0.5, 0}, Color4u(144, 183, 100));    // Light grass
    colorMap.addPoint({0.5, 1}, Color4u(96, 76, 40));       // Dark dirt
    colorMap.addPoint({0.75, 0}, Color4u(96, 76, 40));      // Dark dirt
    colorMap.addPoint({0.75, 1}, Color4u(160, 160, 160));   // Rock
    colorMap.addPoint({1, 0}, Color4u(244, 252, 250));      // Snow
    colorMap.addPoint({1, 1}, Color4u(160, 160, 160));      // Rock
    colorMap.setOrder(3);
}

double Ground::observeAltitudeAt(WorldZone zone, double x, double y) {
	int lvl = _tileSystem.getLod(zone.getInfo().getMaxResolution());
    vec3d offset = zone->getAbsoluteOffset();
    return observeAltitudeAt(offset.x + x, offset.y + y, lvl);
}

void Ground::collectZone(const WorldZone &zone, ICollector &collector, const IResolutionModel &resolutionModel) {
	if (zone.getInfo().getParent().has_value())
		return;

    vec3d offset = zone->getAbsoluteOffset();
    vec3d chunkSize = zone->getDimensions();

    // Tune altitude for the resolution model
    double estimAltitude = observeAltitudeAt(offset.x + chunkSize.x / 2,
                                             offset.y + chunkSize.y / 2, 0);

	ResolutionModelContextWrap wresModel(resolutionModel);
	wresModel.setOffset({ 0, 0, estimAltitude });

    // Find terrains to generate
	for (auto it = _tileSystem.iterate(wresModel, zone); !it.endReached(); ++it) {
		addTerrain(*it, collector);
	}

    updateCache();
}

void Ground::collectZone(const WorldZone &zone, FlatWorld &world,
                         FlatWorldCollector &collector, const IResolutionModel &resolutionModel) {
    collectZone(zone, collector, resolutionModel);
}

void Ground::addWorkerInternal(ITerrainWorker *worker) {
    _internal->_generators.push_back(std::unique_ptr<ITerrainWorker>(worker));
}

double Ground::observeAltitudeAt(double x, double y, int lvl) {
	TileCoordinates key = _tileSystem.getTileCoordinates({ x, y, 0 }, lvl);
	vec3d inTile = _tileSystem.getLocalCoordinates({ x, y, 0 }, lvl);

    const Terrain &terrain = this->provideTerrain(key);
    updateCache();
    return _minAltitude +
           getAltitudeRange() * terrain.getExactHeightAt(inTile.x, inTile.y);
}

inline ICollector::ItemKey terrainToItem(const std::string &key) {
    return ICollector::ItemKeys::inWorld(std::string("_") + key,
                                         ObjectKeys::defaultKey(), 0);
}

void Ground::addTerrain(const TileCoordinates &key, ICollector &collector) {
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
        // material.setKd(1, (double) key._lod / _tileSystem._maxLod, 1 - (double)key._lod / _tileSystem._maxLod);
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

Tile &Ground::provide(const TileCoordinates &key) {
    auto it = _internal->_terrains.find(key);

    if (it == _internal->_terrains.end()) {
        generateTerrain(key);
        it = _internal->_terrains.find(key);
    }

    registerAccess(key, it->second);
    return it->second;
}

void Ground::registerAccess(const TileCoordinates &key, Tile &tile) {
    _internal->_accessCounter++;

    // Remove last access entry for this tile
    if (tile._lastAccess != 0) {
        _internal->_accesses.erase(tile._lastAccess);
    }

    // Make a new entry
    tile._lastAccess = _internal->_accessCounter;
    _internal->_accesses.emplace(tile._lastAccess, key);

    // Update parent
    if (key._lod != 0) {
        auto parentKey = _tileSystem.getParentTileCoordinates(key);
        registerAccess(parentKey, provide(parentKey));
    }
}

Terrain &Ground::provideTerrain(const TileCoordinates &key) {
    return *provide(key)._final;
}

Mesh &Ground::provideMesh(const TileCoordinates &key) {
    auto &meshPtr = provide(key)._mesh;

    if (!meshPtr) {
        generateMesh(key);
    }

    return *meshPtr;
}

optional<Terrain &> Ground::getCachedTerrain(const TileCoordinates &key, int genID) {
    auto it = _internal->_terrains.find(key);

    if (it == _internal->_terrains.end()) {
        return nullopt;
    } else {
        registerAccess(key, it->second);
        return *it->second._cache[genID];
    }
}

std::string Ground::getTerrainDataId(const TileCoordinates &key) const {
    u64 id = static_cast<u64>(key._pos.x & 0x0FFFFFFFu) +
             (static_cast<u64>(key._pos.y & 0x0FFFFFFFu) << 24u) +
             (static_cast<u64>(key._lod & 0xFFu) << 48u);
    return std::to_string(id);
}

void Ground::generateTerrain(const TileCoordinates &key) {
    // Ensure that parent was created
    if (key._lod != 0) {
        provide(_tileSystem.getParentTileCoordinates(key));
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

    double terrainSize = _tileSystem.getTileSize(key._lod).x;
    terrain.setBounds(terrainSize * key._pos.x, terrainSize * key._pos.y,
                      _minAltitude, terrainSize * (key._pos.x + 1),
                      terrainSize * (key._pos.y + 1), _maxAltitude);

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

void Ground::generateMesh(const TileCoordinates &key) {
    // Find required terrains
    Terrain *terrains[3][3];

    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            TileCoordinates jitteredKey = key;
            jitteredKey._pos.x += i - 1;
            jitteredKey._pos.y += j - 1;
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
