#ifndef WORLD_CHUNKSYSTEM_H
#define WORLD_CHUNKSYSTEM_H

#include <worldapi/worldapidef.h>

#include "LODData.h"
#include "Chunk.h"
#include "WorldZone.h"
#include "ChunkID.h"

namespace world {
    class ChunkHandler;

    class PrivateChunkSystem;

    class WORLDAPI_EXPORT ChunkSystem {
    public:
        ChunkSystem();

        ~ChunkSystem();

        void setLODData(int lod, const LODData &data);

        LODData &getLODData(int lod) const;

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
        virtual std::pair<WorldZone, bool> getOrCreateZone(const vec3d &position, int lod = 0);

        std::pair<WorldZone, bool> getOrCreateNeighbourZone(const ChunkHandler &chunk, const vec3i &direction);

        std::vector<std::pair<WorldZone, bool>> getOrCreateChildren(const ChunkHandler &zone);

    private:
        PrivateChunkSystem *_internal;

        int _maxLOD = 4;

        friend class ChunkHandler;

        LODData &getOrCreateLODData(int lod);

        Chunk &getChunk(const ChunkID &id);

        WorldZone getZone(const ChunkID &id);

        /** @returns true if the chunk wasn't created yet */
        bool createChunk(ChunkID id);
    };

    /** Contains a reference to a chunk, and metadata from the
     * chunk system */
    class WORLDAPI_EXPORT ChunkHandler : public IWorldZoneHandler {
    public:
        ChunkHandler(ChunkSystem &system, const ChunkID &id, Chunk &chunk);

        ChunkHandler(const ChunkHandler &other);

        IWorldZoneHandler *clone();

        Chunk &chunk() override;

        const Chunk &getChunk() const override;

        const ChunkID &getID() const override;

        bool operator<(const ChunkHandler &other) const;

        bool operator==(const ChunkHandler &other) const;

        bool hasParent() override;

        WorldZone getParent() const override;

        virtual std::pair<WorldZone, bool> getOrCreateNeighbourZone(const vec3i &direction) override;

        virtual std::vector<std::pair<WorldZone, bool>> getOrCreateChildren() override;

        vec3d getRelativeOffset(const ChunkHandler &other);

        vec3d getAbsoluteOffset();

    private:
        const ChunkID _id;
        Chunk &_chunk;
        ChunkSystem &_system;

        friend class ChunkSystem;
    };
}

#endif //WORLD_CHUNKSYSTEM_H
