#ifndef WORLD_CHUNKSYSTEM_H
#define WORLD_CHUNKSYSTEM_H

#include "core/WorldConfig.h"

#include "IChunkSystem.h"
#include "Chunk.h"
#include "LODGridCoordinates.h"
#include "LODData.h"

namespace world {
class LODGridChunkHandler;

class PLODGridChunkSystem;

class WORLDAPI_EXPORT LODGridChunkSystem : public IChunkSystem {
public:
    LODGridChunkSystem(double baseChunkSize = 1000);

    ~LODGridChunkSystem();

    LODData &getLODData(int lod) const;

    /** Gives the maximum resolution the given LOD can hold. All
     * objects with a better resolution should be stored in a
     * higher LOD.
     * The resolution is given in points per meter. For exemple, a mesh
     * whose faces are 20 cm large has a resolution of 5-6. */
    double getMaxResolution(int lod) const;

    /** Gives the minimum resolution required for an object to be
     * contained in the given LOD. All objects with a worse resolution
     * should be stored in a lower LOD.
     * The resolution is given in points per meter. For exemple, a mesh
     * whose faces are 20 cm large has a resolution of 5-6. */
    double getMinResolution(int lod) const;

    virtual int getLODForResolution(double mrd) const;

    // NAVIGATION
    WorldZone getZone(const vec3d &position) override;

    std::vector<WorldZone> getNeighbourZones(const WorldZone &zone) override;

    std::vector<WorldZone> getChildrenZones(const WorldZone &zone) override;

    Chunk &getChunk(const WorldZone &zone) override;

    void collectZone(const WorldZone &zone, ICollector &collector, IResolutionModel &resolutionModel) override;

private:
    PLODGridChunkSystem *_internal;

    /** If an object has a greater resolution than this value,
     * we can't put it in the minimum LOD. */
    double _subdivResolutionThreshold = 0.5;

    u32 _factor = 2;

    u32 _maxLOD = 6;


    friend class LODGridChunkHandler;

    ChunkKey getChunkKey(const ChunkKey &parent,
                         const LODGridCoordinates &coords) const;

    LODGridCoordinates dropLastPart(const ChunkKey &key) const;

    ChunkKey getParentKey(const ChunkKey &chunkKey) const;

    Chunk &getChunk(const ChunkKey &id);

    WorldZone getZone(const ChunkKey &id);

    /** @returns true if the chunk wasn't created yet */
    bool createChunk(const ChunkKey &key);
};


/** Contains a reference to a chunk, and metadata from the
 * chunk system */
class WORLDAPI_EXPORT LODGridChunkHandler : public IWorldZoneHandler {
public:
    LODGridChunkHandler(LODGridChunkSystem &system, const ChunkKey &id);

    LODGridChunkHandler(const LODGridChunkHandler &other);

    ChunkKey getID() const override;

    optional<WorldZone> getParent() const override;

    vec3d getParentOffset() const override;

    double getMinResolution() const override;

    double getMaxResolution() const override;

    vec3d getDimensions() const override;

private:
    ChunkKey _id;
    LODGridChunkSystem &_system;

    // PRE-CALCULATED DATA
    LODGridCoordinates _coordinates;

    friend class LODGridChunkSystem;
};
} // namespace world

#endif // WORLD_CHUNKSYSTEM_H
