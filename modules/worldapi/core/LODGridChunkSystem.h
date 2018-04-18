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
    QueryResult getChunk(const vec3d &position) override;

    std::vector<QueryResult> getNeighbourChunks(const WorldZone &zone) override;

    std::vector<QueryResult> getChildren(const WorldZone &zone) override;

private:
    PLODGridChunkSystem *_internal;

    /** If an object has a greater resolution than this value,
     * we can't put it in the minimum LOD. */
    double _subdivResolutionThreshold = 0.5;
    int _factor = 4;
    int _maxLOD = 4;

    friend class LODGridChunkHandler;

    ChunkKey getChunkKey(const ChunkKey &parent,
                         const LODGridCoordinates &coords) const;

    Chunk &getChunk(const ChunkKey &id);

    optional<WorldZone> getZone(const ChunkKey &id);

    /** @returns true if the chunk wasn't created yet */
    std::pair<ChunkKey, bool> createChunk(const ChunkKey &parent,
                                          const LODGridCoordinates &coords);
};


/** Contains a reference to a chunk, and metadata from the
 * chunk system */
class WORLDAPI_EXPORT LODGridChunkHandler : public IWorldZoneHandler {
public:
    LODGridChunkHandler(LODGridChunkSystem &system, const ChunkKey &id,
                        Chunk &chunk);

    LODGridChunkHandler(const LODGridChunkHandler &other);

    IWorldZoneHandler *clone() const override;

    Chunk &chunk() override;

    const Chunk &getChunk() const override;

    const ChunkKey &getID() const override;

    bool operator<(const LODGridChunkHandler &other) const;

    bool operator==(const LODGridChunkHandler &other) const;

    bool hasParent() const override;

    optional<WorldZone> getParent() const override;

private:
    ChunkKey _id;
    Chunk &_chunk;
    LODGridChunkSystem &_system;

    friend class LODGridChunkSystem;
};
} // namespace world

#endif // WORLD_CHUNKSYSTEM_H
