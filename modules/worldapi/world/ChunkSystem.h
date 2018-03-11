#ifndef WORLD_CHUNKSYSTEM_H
#define WORLD_CHUNKSYSTEM_H

#include <worldapi/worldapidef.h>

#include "LODData.h"
#include "Chunk.h"

class WORLDAPI_EXPORT ChunkID {
public:
    ChunkID(int x = 0, int y = 0, int z = 0, int lod = 0);
    ChunkID(const maths::vec3i & pos, int lod = 0);
    ChunkID(const ChunkID & other);
    ~ChunkID();

    const maths::vec3i & getPosition3D() const;
    int getLOD() const { return _lod; }

    bool operator<(const ChunkID & other) const;
    bool operator==(const ChunkID & other) const;
private:
    maths::vec3i _pos;
    int _lod;
};

class ChunkSystem;

// TODO rename ?
/** Contains a reference to a chunk, and metadata from the
 * chunk system */
class WORLDAPI_EXPORT ChunkNode {
public:
    ChunkID _id;
    Chunk& _chunk;
    ChunkSystem& _system;

    ChunkNode(ChunkSystem& system, const ChunkID &id, Chunk& chunk);
    ChunkNode(const ChunkNode &other);

    bool operator<(const ChunkNode &other) const;
    bool operator==(const ChunkNode &other) const;
};


class PrivateChunkSystem;

class WORLDAPI_EXPORT ChunkSystem {
public:
    ChunkSystem();
    ~ChunkSystem();

    void setLODData(int lod, const LODData & data);
    LODData & getLODData(int lod) const;

    /** Give the maximum detail size for all the chunks with
     * this level of detail.
     * All objects with a bigger detail size should be in a larger
     * chunk.
     * The detail size is given in meters. */
    double getMaxDetailSize(int lod) const;

    /** Give the minimum detail size for all the chunks with
     * this level of detail.
     * All objects with a smaller detail size should be in a
     * smaller chunk.
     * The detail size is given in meters. */
    double getMinDetailSize(int lod) const;

    virtual int getLODForDetailSize(double detailSize) const;

    // NAVIGATION
    virtual std::pair<ChunkNode, bool> getOrCreateChunkID(const maths::vec3d& position, int lod = 0);
    virtual std::pair<ChunkNode, bool> getOrCreateNeighbourID(const ChunkNode &chunk, const maths::vec3i &direction);
private:
    PrivateChunkSystem* _internal;

    int _maxLOD;

    LODData& getOrCreateLODData(int lod);
    Chunk & getChunk(const ChunkID &id);
    ChunkNode getChunkNode(const ChunkID &id);
    /** @returns true if the chunk wasn't created yet */
    bool createChunk(ChunkID id);
};

#endif //WORLD_CHUNKSYSTEM_H
