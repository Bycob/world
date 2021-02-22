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
#include "world/core/GridStorage.h"
#include "world/core/GridStorageReducer.h"
#include "MultilayerGroundTexture.h"
#include "CachedTextureProvider.h"

namespace world {


struct WorkerConstraints {
    int _lodMin = 0;
    int _lodMax = 0xff;
};


struct WorkerEntry {
    WorkerConstraints _constraints;
    std::unique_ptr<ITerrainWorker> _worker;

    WorkerEntry(ITerrainWorker *worker) : _worker(worker) {}
};

using Tile = HeightmapGround::Tile;


// Utility class
class GroundContext : public ITileContext {
public:
    HeightmapGround *_ground;
    WorkerEntry *_entry;
    Tile *_tile;
    ExplorationContext _ctx;

    GroundContext(HeightmapGround *ground, WorkerEntry *entry, Tile *tile,
                  const ExplorationContext &ctx)
            : _ground(ground), _entry(entry), _tile(tile), _ctx(ctx) {}

    Tile &getTile() const override { return *_tile; }

    TileCoordinates getCoords() const override { return _tile->_key; }

    TileCoordinates getParentCoords() const override {
        return _ground->_tileSystem.getParentTileCoordinates(_tile->_key);
    }

    const ExplorationContext &getExplorationContext() const override {
        return _ctx;
    }
};


class PGround {
public:
    PGround(TileSystem &ts) : _reducer(ts, ts._maxLod * 50) {
        _terrains.setReducer(&_reducer);
    }

    GridStorageReducer _reducer;
    GridStorage<HeightmapGroundTile> _terrains;
    std::list<WorkerEntry> _generators;
};


WORLD_REGISTER_CHILD_CLASS(GroundNode, HeightmapGround, "HeightmapGround")

// Idees d'ameliorations :
// - Systeme de coordonnees semblable a celui du chunk system : les
// coordonnees sont relatives au niveau du dessus
// (Pour ce point, on generalisera peut-etre le system de chunk emboite
// dans une classe avec des templates)

HeightmapGround::HeightmapGround(double unitSize, double minAltitude,
                                 double maxAltitude)
        : _minAltitude(minAltitude), _maxAltitude(maxAltitude),
          _tileSystem(
              5, vec3d(_textureRes * _texPixSize, _textureRes * _texPixSize, 0),
              vec3d(unitSize, unitSize, 0)) {
    _internal = new PGround(_tileSystem);
    _internal->_terrains._cache.setChild(_cache, "terrains");
}

HeightmapGround::~HeightmapGround() { delete _internal; }

IAtmosphericProvider *HeightmapGround::getAtmosphericProvider() {
    for (auto &worker : _internal->_generators) {
        auto *ap = dynamic_cast<IAtmosphericProvider *>(worker._worker.get());
        if (ap != nullptr) {
            return ap;
        }
    }
    return nullptr;
}

void HeightmapGround::setDefaultWorkerSet() {
    // setLodRange(addWorker<PerlinTerrainGenerator>(3, 4., 0.35), 0, 0);
    // setLodRange(addWorker<DiamondSquareTerrain>(0.5), 1, 5);
    addWorker<PerlinTerrainGenerator>(3, 4., 0.35).setMaxOctaveCount(6);
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
    for (auto &entry : _internal->_generators) {
        // TODO not rely on addresses to check worker equality ?
        if (entry._worker.get() == &worker) {
            entry._constraints._lodMin = minLod;
            entry._constraints._lodMax = maxLod;
            break;
        }
    }
}

double HeightmapGround::observeAltitudeAt(double x, double y, double resolution,
                                          const ExplorationContext &ctx) {
    int lvl = _tileSystem.getLod(resolution);
    return observeAltitudeAt(x, y, lvl, ctx);
}

void HeightmapGround::collect(ICollector &collector,
                              const IResolutionModel &resolutionModel,
                              const ExplorationContext &ctx) {

    BoundingBox bbox = resolutionModel.getBounds();

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
    generateTerrains(toGenerate, ctx);

    for (auto &coord : toCollect) {
        addTerrain(coord, collector, ctx);
    }

    // std::cout << "Ground before reducing: " << _internal->_terrains.size();
    _internal->_reducer.reduceStorage();
    // std::cout << ", Ground after reducing: " << _internal->_terrains.size()
    //          << std::endl;
}

void HeightmapGround::paintTexture(const vec2d &origin, const vec2d &size,
                                   const vec2d &resolutionRange,
                                   const Image &img,
                                   const ExplorationContext &ctx) {
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
                // TODO context or not context?
                ImageUtils::paintTexturef(
                    provideTerrain(current, ctx).getTexture(), img,
                    {imgCoords.x, imgCoords.y}, {imgSize.x, imgSize.y});
            }
        }
    }
}

void HeightmapGround::write(WorldFile &wf) const {
    wf.addDouble("minAltitude", _minAltitude);
    wf.addDouble("maxAltitude", _maxAltitude);
    wf.addInt("terrainRes", _terrainRes);
    wf.addInt("textureRes", _textureRes);
    wf.addInt("texPixSize", _texPixSize);

    wf.addStruct("tileSystem", _tileSystem);

    wf.addArray("workers");

    for (auto &generator : _internal->_generators) {
        wf.addToArray("workers", generator._worker->serializeSubclass());
    }
}

void HeightmapGround::read(const WorldFile &wf) {
    wf.readDoubleOpt("minAltitude", _minAltitude);
    wf.readDoubleOpt("maxAltitude", _maxAltitude);
    wf.readIntOpt("terrainRes", _terrainRes);
    wf.readIntOpt("textureRes", _textureRes);
    wf.readIntOpt("texPixSize", _texPixSize);

    wf.readStruct("tileSystem", _tileSystem);
    _tileSystem._bufferRes.x = _tileSystem._bufferRes.y =
        _textureRes * _texPixSize;
    // TODO change 50 to a controllable parameter
    // TODO put this update in a method so that everything is uniform
    _internal->_reducer.setMaxInstances(50 * _tileSystem._maxLod);

    for (auto it = wf.readArray("workers"); !it.end(); ++it) {
        addWorkerInternal(readSubclass<ITerrainWorker>(*it));
    }
}

void HeightmapGround::addWorkerInternal(ITerrainWorker *worker) {
    _internal->_generators.emplace_back(worker);
    const size_t workerId = _internal->_generators.size();
    const std::string workerName = "worker" + std::to_string(workerId);

    auto *storage = worker->getStorage();
    auto *cache = worker->getCache();

    if (storage != nullptr) {
        _internal->_reducer.registerStorage(storage);
        storage->getCache().setChild(_cache, workerName);
    }

    if (cache != nullptr) {
        cache->setChild(_cache, workerName);

        if (storage != nullptr) {
            storage->getCache().setChild(*cache, "storage");
        }
    }
}

ITerrainWorker *HeightmapGround::getWorkerInternal(const std::type_info &type) {
    for (auto &entry : _internal->_generators) {
        if (typeid(*entry._worker).hash_code() == type.hash_code()) {
            return entry._worker.get();
        }
    }

    return nullptr;
}

double HeightmapGround::observeAltitudeAt(double x, double y, int lvl,
                                          const ExplorationContext &ctx) {
    TileCoordinates key = _tileSystem.getTileCoordinates({x, y, 0}, lvl);
    vec3d inTile = _tileSystem.getLocalCoordinates({x, y, 0}, lvl);

    const Terrain &terrain = this->provideTerrain(key, ctx);
    return _minAltitude +
           getAltitudeRange() * terrain.getExactHeightAt(inTile.x, inTile.y);
}

inline ItemKey terrainToItem(const std::string &key) { return {"_" + key}; }

void HeightmapGround::addTerrain(const TileCoordinates &key,
                                 ICollector &collector,
                                 const ExplorationContext &ctx) {
    ItemKey itemKey = terrainToItem(getTerrainDataId(key));
    Tile &tile = provide(key, ctx);
    Terrain &terrain = tile._terrain;

    if (collector.hasChannel<SceneNode>() && collector.hasChannel<Terrain>()) {
        // Collect as terrains (more optimized e.g. for game engines)
        auto &terrainChannel = collector.getChannel<Terrain>();
        terrainChannel.put(itemKey, terrain, ctx);

        auto &objChannel = collector.getChannel<SceneNode>();
        SceneNode node(ctx(itemKey).str());
        objChannel.put(itemKey, node, ctx);

    } else if (collector.hasChannel<SceneNode>() &&
               collector.hasChannel<Mesh>()) {
        // Collect as meshes (more generic)
        auto &objChannel = collector.getChannel<SceneNode>();
        auto &meshChannel = collector.getChannel<Mesh>();

        if (!objChannel.has(itemKey)) {
            // Relocate the terrain
            auto &bbox = terrain.getBoundingBox();
            vec3d offset = bbox.getLowerBound();

            // Create the mesh
            meshChannel.put(itemKey, provideMesh(key, ctx), ctx);

            SceneNode object(ctx(itemKey).str());
            object.setPosition(offset);

            // Create the material
            Material material("terrain");
            material.setKd(1, 1, 1);
// #define DEBUG_COLOR
#ifdef DEBUG_COLOR
            double value = (double)key._lod / _tileSystem._maxLod;
            double _void;
            material.setKd(modf(0.5 + value, &_void), 1 - value, value);
#endif
            material.setMapKd("texture01");

            // Retrieve the texture
            auto &texture = terrain.getTexture();

            if (collector.hasChannel<Material>()) {
                auto &matChan = collector.getChannel<Material>();
                object.setMaterialID(ctx(itemKey).str());

                if (collector.hasChannel<Image>()) {
                    auto &imageChan = collector.getChannel<Image>();
                    material.setMapKd(ctx(itemKey).str());
                    imageChan.put(itemKey, texture, ctx);
                }

                matChan.put(itemKey, material, ctx);
            }

            objChannel.put(itemKey, object, ctx);
        }
    }

    int id = 0;
    for (auto &gen : _internal->_generators) {
        ExplorationContext childCtx(ctx);
        childCtx.appendPrefix("worker" + std::to_string(id));
        GroundContext gctx(this, &gen, &tile, childCtx);
        gen._worker->collectTile(collector, gctx);
        ++id;
    }
}

// ==== ACCESS

Tile &HeightmapGround::provide(const TileCoordinates &key,
                               const ExplorationContext &ctx) {

    Tile &tile = _internal->_terrains.getOrCreateCallback(
        key,
        [this, &key, &ctx](HeightmapGroundTile &tile) {
            std::set<TileCoordinates> coords{key};
            addNotGeneratedParents(coords);
            generateTerrains(coords, ctx);
        },
        key, _terrainRes);

    return tile;
}

Terrain &HeightmapGround::provideTerrain(const TileCoordinates &key,
                                         const ExplorationContext &ctx) {
    return provide(key, ctx)._terrain;
}

Mesh &HeightmapGround::provideMesh(const TileCoordinates &key,
                                   const ExplorationContext &ctx) {
    auto &mesh = provide(key, ctx)._mesh;

    if (mesh.empty()) {
        generateMesh(key, ctx);
    }

    return mesh;
}

bool HeightmapGround::isGenerated(const TileCoordinates &key) {
    return _internal->_terrains.has(key);
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

                if (keys.find(pkey) == keys.end() && !isGenerated(pkey)) {
                    temp.insert(pkey);
                }
            }
        }

        keys.insert(temp.begin(), temp.end());
    } while (!temp.empty());
}

void HeightmapGround::generateTerrains(const std::set<TileCoordinates> &keys,
                                       const ExplorationContext &ctx) {

    // Each lod is generated separately to ensure parents are
    // created before generating children
    typedef std::vector<Tile *> generateTiles_t;
    std::vector<generateTiles_t> lods(_tileSystem._maxLod + 1);

    for (const TileCoordinates &key : keys) {
        Tile &tile = _internal->_terrains.getOrCreate(key, key, _terrainRes);

        if (!_internal->_terrains.readTileFromCache(key, tile)) {
            lods[key._lod].push_back(&tile);

            // set texture
            tile._terrain.setTexture(
                Image(_textureRes, _textureRes, ImageType::RGB));
        }

        // set bounds
        double terrainSize = _tileSystem.getTileSize(key._lod).x;
        tile._terrain.setBounds(terrainSize * key._pos.x,
                                terrainSize * key._pos.y, _minAltitude,
                                terrainSize * (key._pos.x + 1),
                                terrainSize * (key._pos.y + 1), _maxAltitude);
    }

    int lod = 0;

    for (auto &generatedTiles : lods) {

        // Generation
        int gid = 0;

        for (auto &entry : _internal->_generators) {
            auto &generator = entry._worker;
            auto &constraints = entry._constraints;

            GroundContext context(this, &entry, nullptr, ctx);
            context._ctx.appendPrefix("worker" + std::to_string(gid));

            // check if constraints are fullfilled
            bool doGeneration =
                constraints._lodMin <= lod && constraints._lodMax >= lod;

            if (doGeneration) {
                for (auto &tile : generatedTiles) {
                    Terrain &terrain = tile->_terrain;

                    context._tile = tile;
                    generator->processTile(context);
                }

                generator->flush();
            }
            ++gid;
        }

        ++lod;
    }
}

void HeightmapGround::generateMesh(const TileCoordinates &key,
                                   const ExplorationContext &ctx) {
    // Find required terrains
    Terrain *terrains[3][3];

    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            TileCoordinates jitteredKey = key;
            jitteredKey._pos.x += i - 1;
            jitteredKey._pos.y += j - 1;
            terrains[i][j] = &provideTerrain(jitteredKey, ctx);
        }
    }

    // Fill mesh
    // Same as Terrain::createMesh, but may become different
    // + here we compute normals a different way (for tiling to be acceptable).
    Mesh &mesh = provide(key, ctx)._mesh;

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
