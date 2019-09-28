#include "HeightmapGround.h"

#include <map>
#include <unordered_map>
#include <memory>
#include <list>

#include "world/core/WorldTypes.h"
#include "world/assets/SceneNode.h"
#include "world/assets/Material.h"
#include "world/assets/ImageUtils.h"
#include "world/math/MathsHelper.h"
#include "ApplyParentTerrain.h"
#include "PerlinTerrainGenerator.h"
#include "ReliefMapModifier.h"
#include "AltitudeTexturer.h"
#include "SimpleTexturer.h"
#include "TerrainOps.h"
#include "world/core/Profiler.h"
#include "DiamondSquareTerrain.h"

namespace world {

struct HeightmapGround::Tile {
    TileCoordinates _key;
    std::vector<optional<Terrain>> _cache;
    std::unique_ptr<Terrain> _terrain;
    std::unique_ptr<Mesh> _mesh;
    u64 _lastAccess;
};

struct WorkerConstraints {
    int _lodMin = 0;
    int _lodMax = 0xff;
};

using Tile = HeightmapGround::Tile;

// Utility class
class GroundContext : public ITileContext {
public:
    TileCoordinates _key;
    int _genID;
    HeightmapGround &_ground;

    bool _registerState = false;

    GroundContext(HeightmapGround &ground, const TileCoordinates &key,
                  int genID)
            : _key(key), _genID(genID), _ground(ground) {}

    Terrain &getTerrain() const override {
        return _ground.provideTerrain(_key);
    }

    Image &getTexture() const override {
        return _ground.provideTerrain(_key).getTexture();
    }

    optional<const Terrain &> getNeighbour(int x, int y) const override {
        TileCoordinates nKey{_key._pos.x + x, _key._pos.y + y, 0, _key._lod};
        auto ret = _ground.lookUpTile(nKey);

        if (ret) {
            auto &terrain = ret->_cache[_genID];

            if (terrain) {
                return *terrain;
            } else {
                return nullopt;
            }
        } else {
            return nullopt;
        }
    }

    optional<const Terrain &> getParent() const override {
        if (_key._lod == 0)
            return nullopt;

        auto parentId = _ground._tileSystem.getParentTileCoordinates(_key);
        auto &tile = *_ground.lookUpTile(parentId);
        auto &terrain = tile._cache[_genID];

        if (terrain) {
            return *terrain;
        } else {
            return nullopt;
        }
    }

    int getParentCount() const override { return _key._lod; }

    vec2i getTileCoords() const override {
        return static_cast<vec2i>(_key._pos);
    }

    void registerCurrentState() override { _registerState = true; }
};

class PGround {
public:
    PGround() : _terrains(65536), _accesses(65536) {}

    std::unordered_map<TileCoordinates, Tile> _terrains;
    std::list<std::pair<std::unique_ptr<ITerrainWorker>, WorkerConstraints>>
        _generators;

    u64 _accessCounter = 0;
    std::unordered_map<u64, TileCoordinates> _accesses;
};

// Idees d'ameliorations :
// - Systeme de coordonnees semblable a celui du chunk system : les
// coordonnees sont relatives au niveau du dessus
// (Pour ce point, on generalisera peut-etre le system de chunk emboite
// dans une classe avec des templates)

HeightmapGround::HeightmapGround(double unitSize, double minAltitude,
                                 double maxAltitude)
        : _internal(new PGround()), _minAltitude(minAltitude),
          _maxAltitude(maxAltitude),
          _tileSystem(5, vec3d(_textureRes, _textureRes, 0),
                      vec3d(unitSize, unitSize, 0)) {}

HeightmapGround::~HeightmapGround() { delete _internal; }

void HeightmapGround::setDefaultWorkerSet() {
    // setLodRange(addWorker<PerlinTerrainGenerator>(3, 4., 0.35), 0, 0);
    // setLodRange(addWorker<DiamondSquareTerrain>(0.5), 1, 5);
    addWorker<PerlinTerrainGenerator>(3, 4., 0.35)
            .setMaxOctaveCount(6);
    auto &map = addWorker<CustomWorldRMModifier>();
    map.setRegion({0, 0}, 10000, 3, 0.1, 0.3);
    map.setRegion({0, 0}, 6000, 0.7, 1.6, 0.8);

    // Texturer
    auto &texturer = addWorker<AltitudeTexturer>();
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

void HeightmapGround::setLodRange(const ITerrainWorker &worker, int minLod,
                                  int maxLod) {
    for (auto &genPair : _internal->_generators) {
        // TODO not rely on addresses to check worker equality ?
        if (genPair.first.get() == &worker) {
            genPair.second._lodMin = minLod;
            genPair.second._lodMax = maxLod;
            break;
        }
    }
}

double HeightmapGround::observeAltitudeAt(double x, double y,
                                          double resolution) {
    int lvl = _tileSystem.getLod(resolution);
    return observeAltitudeAt(x, y, lvl);
}

void HeightmapGround::collect(ICollector &collector,
                              const IResolutionModel &resolutionModel,
                              const ExplorationContext &ctx) {

    BoundingBox bbox = resolutionModel.getBounds();

    // Tune altitude for the resolution model
    /*double estimAltitude = observeAltitudeAt(offset.x + chunkSize.x / 2,
                                             offset.y + chunkSize.y / 2, 0);

    ResolutionModelContextWrap wresModel(resolutionModel);
    wresModel.setOffset({0, 0, estimAltitude});*/

    // Find terrains to generate
    std::set<TileCoordinates> toCollect;
    std::set<TileCoordinates> toGenerate;

    for (auto it = _tileSystem.iterate(resolutionModel, bbox); !it.endReached();
         ++it) {

        toCollect.insert(*it);

        if (!isGenerated(*it)) {
            toGenerate.insert(*it);
        }
    }

    addNotGeneratedParents(toGenerate);
    generateTerrains(toGenerate);

    for (auto &coord : toCollect) {
        addTerrain(coord, collector);
    }

    updateCache();
}

void HeightmapGround::paintTexture(const vec2d &origin, const vec2d &size,
                                   const vec2d &resolutionRange,
                                   const Image &img) {
    const int minLod = _tileSystem.getLod(resolutionRange.x);
    const int maxLod = _tileSystem.getLod(resolutionRange.y);

    const vec3d min{origin.x, origin.y, 0};
    const vec3d max{origin.x + size.x, origin.y + size.y, 0};

    for (int lod = minLod; lod <= maxLod; ++lod) {
        TileCoordinates tileMin = _tileSystem.getTileCoordinates(min, lod);
        TileCoordinates tileMax = _tileSystem.getTileCoordinates(max, lod);
        vec3d localMin = _tileSystem.getLocalCoordinates(min, lod);
        vec3d localMax = _tileSystem.getLocalCoordinates(max, lod);
        vec3d tileSize = _tileSystem.getTileSize(lod);
        vec3i tileDist = tileMax._pos - tileMin._pos;

        vec3d imgSize = tileSize * tileDist + localMax - localMin;

        for (int x = tileMin._pos.x; x <= tileMax._pos.x; ++x) {
            for (int y = tileMin._pos.y; y <= tileMax._pos.y; ++y) {
                TileCoordinates current{x, y, 0, lod};
                vec3d imgCoords =
                    (tileMin._pos - current._pos) * tileSize + localMin;
                ImageUtils::paintTexturef(provideTerrain(current).getTexture(),
                                          img, {imgCoords.x, imgCoords.y},
                                          {imgSize.x, imgSize.y});
            }
        }
    }
}

void HeightmapGround::addWorkerInternal(ITerrainWorker *worker) {
    _internal->_generators.emplace_back(std::unique_ptr<ITerrainWorker>(worker),
                                        WorkerConstraints{});
}

double HeightmapGround::observeAltitudeAt(double x, double y, int lvl) {
    TileCoordinates key = _tileSystem.getTileCoordinates({x, y, 0}, lvl);
    vec3d inTile = _tileSystem.getLocalCoordinates({x, y, 0}, lvl);

    const Terrain &terrain = this->provideTerrain(key);
    updateCache();
    return _minAltitude +
           getAltitudeRange() * terrain.getExactHeightAt(inTile.x, inTile.y);
}

inline ItemKey terrainToItem(const std::string &key) { return {"_" + key}; }

void HeightmapGround::addTerrain(const TileCoordinates &key,
                                 ICollector &collector) {
    ItemKey itemKey = terrainToItem(getTerrainDataId(key));
    Terrain &terrain = this->provideTerrain(key);

    if (collector.hasChannel<SceneNode>() && collector.hasChannel<Mesh>()) {

        auto &objChannel = collector.getChannel<SceneNode>();
        auto &meshChannel = collector.getChannel<Mesh>();

        if (!objChannel.has(itemKey)) {
            // Relocate the terrain
            auto &bbox = terrain.getBoundingBox();
            vec3d offset = bbox.getLowerBound();

            // Create the mesh
            meshChannel.put(itemKey, provideMesh(key));

            SceneNode object(itemKey.str());
            object.setPosition(offset);

            // Create the material
            Material material("terrain");
            material.setKd(1, 1, 1);
// #define DEBUG_COLOR
#ifdef DEBUG_COLOR
            double value = (double) key._lod / _tileSystem._maxLod;
            double _void;
            material.setKd(modf(0.5 + value, &_void), 1 - value, value);
#endif
            material.setMapKd("texture01");

            // Retrieve the texture
            auto &texture = terrain.getTexture();

            if (collector.hasChannel<Material>()) {
                auto &matChan = collector.getChannel<Material>();
                object.setMaterialID(itemKey.str());

                if (collector.hasChannel<Image>()) {
                    auto &imageChan = collector.getChannel<Image>();
                    material.setMapKd(itemKey.str());
                    imageChan.put(itemKey, texture);
                }

                matChan.put(itemKey, material);
            }

            objChannel.put(itemKey, object);
        }
    }
}

void HeightmapGround::updateCache() {
    if (_manageCache && _internal->_terrains.size() > _maxCacheSize) {

        // We shrink one third of the memory
        auto count = _internal->_terrains.size() - _maxCacheSize * 2 / 3;

        std::cout << "Dropping memory : " << count
                  << " removed." << std::endl;

        while (count != 0) {
            auto accessEntry = _internal->_accesses.begin();

            // Free memory
            // TODO call to a drop manager
            _internal->_terrains.erase(accessEntry->second);

            // Remove access entry
            _internal->_accesses.erase(accessEntry);

            count--;
        }
    }
}


// ==== ACCESS

void HeightmapGround::registerAccess(const TileCoordinates &key, Tile &tile) {
    if (!_manageCache)
        return;

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

Tile &HeightmapGround::provide(const TileCoordinates &key) {
    auto it = _internal->_terrains.find(key);

    if (it == _internal->_terrains.end()) {
        std::set<TileCoordinates> coords{key};
        addNotGeneratedParents(coords);
        generateTerrains(coords);
        it = _internal->_terrains.find(key);
    }

    auto &tile = it->second;
    registerAccess(key, tile);
    return tile;
}

Terrain &HeightmapGround::provideTerrain(const TileCoordinates &key) {
    return *provide(key)._terrain;
}

Mesh &HeightmapGround::provideMesh(const TileCoordinates &key) {
    auto &meshPtr = provide(key)._mesh;

    if (!meshPtr) {
        generateMesh(key);
    }

    return *meshPtr;
}

optional<HeightmapGround::Tile &> HeightmapGround::lookUpTile(
    const TileCoordinates &key) {
    auto it = _internal->_terrains.find(key);

    if (it == _internal->_terrains.end()) {
        return nullopt;
    } else {
        registerAccess(key, it->second);
        return it->second;
    }
}

bool HeightmapGround::isGenerated(const TileCoordinates &key) {
    return _internal->_terrains.find(key) != _internal->_terrains.end();
}


std::string HeightmapGround::getTerrainDataId(
    const TileCoordinates &key) const {
    u64 id = static_cast<u64>(key._pos.x & 0x0FFFFFFFu) +
             (static_cast<u64>(key._pos.y & 0x0FFFFFFFu) << 24u) +
             (static_cast<u64>(key._lod & 0xFFu) << 48u);
    return std::to_string(id);
}


// ==== GENERATION
void HeightmapGround::addNotGeneratedParents(std::set<TileCoordinates> &keys) {
    std::set<TileCoordinates> temp;

    do {
        temp.clear();

        for (const TileCoordinates &key : keys) {
            if (key._lod != 0) {
                auto pkey = _tileSystem.getParentTileCoordinates(key);

                if (keys.find(pkey) == keys.end() && !isGenerated(key)) {
                    temp.insert(pkey);
                }
            }
        }

        keys.insert(temp.begin(), temp.end());
    } while (temp.size() != 0);
}

void HeightmapGround::generateTerrains(const std::set<TileCoordinates> &keys) {

    typedef std::vector<Tile *> generateTiles_t;
    std::vector<generateTiles_t> lods(_tileSystem._maxLod + 1);

    for (const TileCoordinates &key : keys) {
        Tile *tile = &(_internal->_terrains[key] =
                           Tile{key,
                                std::vector<optional<Terrain>>(
                                    _internal->_generators.size(), nullopt),
                                std::make_unique<Terrain>(_terrainRes),
                                std::unique_ptr<Mesh>(), 0});
        lods[key._lod].push_back(tile);
    }

    // Each lod is generated separately to ensure parents are
    // created before generating children
    int lod = 0;

    for (auto &generatedTiles : lods) {

        // Allocation of terrain and textures
        for (auto &tile : generatedTiles) {
            const auto &key = tile->_key;
            Terrain &terrain = *tile->_terrain;

            terrain.setTexture(
                Image(_textureRes, _textureRes, ImageType::RGB));

            double terrainSize = _tileSystem.getTileSize(key._lod).x;
            terrain.setBounds(terrainSize * key._pos.x,
                              terrainSize * key._pos.y, _minAltitude,
                              terrainSize * (key._pos.x + 1),
                              terrainSize * (key._pos.y + 1), _maxAltitude);
        }

        // Generation
        GroundContext context(*this, {}, 0);

        for (auto &genPair : _internal->_generators) {
            auto &generator = genPair.first;
            auto &constraints = genPair.second;

            // check if constraints are fullfilled
            bool doGeneration =
                constraints._lodMin <= lod && constraints._lodMax >= lod;

            if (doGeneration) {
                for (auto &tile : generatedTiles) {
                    Terrain &terrain = *tile->_terrain;
                    std::vector<optional<Terrain>> &cache = tile->_cache;

                    context._key = tile->_key;
                    generator->processTile(context);

                    if (context._registerState) {
                        cache[context._genID] = terrain;
                        context._registerState = false;
                    }
                }

                generator->flush();
            }

            context._genID++;
        }

        ++lod;
    }
}

void HeightmapGround::generateMesh(const TileCoordinates &key) {
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
