#ifndef WORLD_CHUNKSYSTEM_H
#define WORLD_CHUNKSYSTEM_H

#include <worldapi/worldapidef.h>

#include "LODData.h"
#include "Chunk.h"

class WORLDAPI_EXPORT ChunkID {
public:
    static ChunkID NONE;

    ChunkID(int x, int y, int z, int lod = 0);
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

class WorldZone;

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
    virtual std::pair<WorldZone, bool> getOrCreateZone(const maths::vec3d &position, int lod = 0);
    std::pair<WorldZone, bool> getOrCreateNeighbourZone(const WorldZone &chunk, const maths::vec3i &direction);
private:
    PrivateChunkSystem* _internal;

    int _maxLOD;

    friend class WorldZone;

    LODData& getOrCreateLODData(int lod);
    Chunk & getChunk(const ChunkID &id);
    WorldZone getZone(const ChunkID &id);
    /** @returns true if the chunk wasn't created yet */
    bool createChunk(ChunkID id);
};

/** Contains a reference to a chunk, and metadata from the
 * chunk system */
class WORLDAPI_EXPORT WorldZone {
public:
    WorldZone(ChunkSystem& system, const ChunkID &id, Chunk& chunk);
    WorldZone(const WorldZone &other);

    Chunk& chunk();
    // TODO est-ce que ceci doit etre accessible ?
    const ChunkID &getID() const;

    bool operator<(const WorldZone &other) const;
    bool operator==(const WorldZone &other) const;

    bool hasParent();
    WorldZone getParent() const;
    maths::vec3d getRelativeOffset(const WorldZone &other);
    maths::vec3d getAbsoluteOffset();
private:
    const ChunkID _id;
    Chunk& _chunk;
    ChunkSystem& _system;
};


#endif //WORLD_CHUNKSYSTEM_H
