#ifndef WORLD_TERRAINNODE_H
#define WORLD_TERRAINNODE_H

#include "world/core/WorldConfig.h"

#include <map>
#include <utility>
#include <functional>

#include "world/core/TileSystem.h"
#include "world/flat/IGround.h"
#include "Terrain.h"
#include "ITerrainWorker.h"

namespace world {

class PGround;

/** This class manages an infinite ground with as much details
 * as we want. The access method are quite similar to the ones
 * used on the WorldObjects : you can collect parts of the ground
 * by specifying which part of the world you're wanting to get
 * content from. */
class WORLDAPI_EXPORT HeightmapGround : public GroundNode {
public:
    struct Tile;

    HeightmapGround(double unitSize = 6000, double minAltitude = -2000,
                    double maxAltitude = 4000);

    ~HeightmapGround() override;

    // PARAMETERS
    // TODO constraints
    void setMaxAltitude(double max) { _maxAltitude = max; }

    void setMinAltitude(double min) { _minAltitude = min; }

    double getMaxAltitude() const { return _maxAltitude; }

    double getMinAltitude() const { return _minAltitude; }

    double getAltitudeRange() const { return _maxAltitude - _minAltitude; }

    // TERRAIN WORKERS
    /** Adds a default worker set to generate heightmaps in the
     * ground. This method is for quick-setup purpose. */
    void setDefaultWorkerSet();

    /** Creates and adds an ITerrainWorker to the Ground. The
     * ITerrainWorkers belong to the core of the generation process.
     * Each part of the ground is generated by a chain of worker,
     * each of which adds one feature after the other to the terrain.
     * */
    template <typename T, typename... Args> T &addWorker(Args &&... args);

    // EXPLORATION
    double observeAltitudeAt(double x, double y, double resolution) override;

    void collect(ICollector &collector,
                 const IResolutionModel &resolutionModel) override;

private:
    PGround *_internal;

    // Param�tres
    /** L'altitude minimum du monde. Le niveau de la mer est fix� � 0. */
    double _minAltitude;
    /** L'altitude maximum du monde. Le niveau de la mer est fix� � 0. */
    double _maxAltitude;

    int _terrainRes = 33;
    /** Texture resolution, relatively to the terrain resolution */
    int _textureRes = 8;

    TileSystem _tileSystem;

    u32 _maxCacheSize = 2000;

    // WORKER
    void addWorkerInternal(ITerrainWorker *worker);

    double observeAltitudeAt(double x, double y, int lvl);

    /** Replace a parent terrain by its children in the collector */
    void replaceTerrain(const TileCoordinates &key, ICollector &collector);

    void addTerrain(const TileCoordinates &key, ICollector &collector);

    /** Updates the cache, free old used memory if needed (by saving
     * the terrains to files or discard them) */
    void updateCache();


    // ACCESS
    HeightmapGround::Tile &provide(const TileCoordinates &key);

    void registerAccess(const TileCoordinates &key, Tile &tile);

    Terrain &provideTerrain(const TileCoordinates &key);

    Mesh &provideMesh(const TileCoordinates &key);

    optional<Terrain &> getCachedTerrain(const TileCoordinates &key, int genID);


    // DATA
    /** Gets a unique string id for the given tile in the Ground. */
    std::string getTerrainDataId(const TileCoordinates &key) const;


    // GENERATION
    /** Generate the terrain with given coordinates. Assume that:
     * - the terrain wasn't generated yet,
     * - the terrains with higher level at the same place are already
     * generated.*/
    void generateTerrain(const TileCoordinates &key);

    void generateMesh(const TileCoordinates &key);

    friend class PGround;
    friend class GroundContext;
};

template <typename T, typename... Args>
T &HeightmapGround::addWorker(Args &&... args) {
    T *worker = new T(args...);
    addWorkerInternal(worker);
    return *worker;
};
} // namespace world

#endif // WORLD_TERRAINNODE_H
