#ifndef WORLD_CHUNKSYSTEM_H
#define WORLD_CHUNKSYSTEM_H

#include <worldapi/worldapidef.h>

#include "WorldKeys.h"
#include "LODData.h"
#include "Chunk.h"
#include "IChunkSystem.h"
#include "LODGridCoordinates.h"

namespace world {
    class LODGridChunkHandler;

    class WORLDAPI_EXPORT LODGridChunkSystem : public IChunkSystem {
    public:
        LODGridChunkSystem();

        ~LODGridChunkSystem();

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
        QueryResult getChunk(const vec3d &position) override;

        QueryResult getNeighbourChunk(const WorldZone &zone, const vec3i &direction) override;

        std::vector<QueryResult> getChildren(const WorldZone &zone) override;

    private:
        class Impl;
        Impl *_internal;

        int _maxLOD = 6;

        friend class LODGridChunkHandler;

        LODData &getOrCreateLODData(int lod);

        ChunkKey getChunkKey(const ChunkKey &parent, const LODGridCoordinates &coords) const;

        Chunk &getChunk(const ChunkKey &id);

        WorldZone getZone(const ChunkKey &id);

        /** @returns true if the chunk wasn't created yet */
        std::pair<ChunkKey, bool> createChunk(const ChunkKey &parent, const LODGridCoordinates &coords);
    };


    /** Contains a reference to a chunk, and metadata from the
     * chunk system */
    class WORLDAPI_EXPORT LODGridChunkHandler : public IWorldZoneHandler {
    public:
        LODGridChunkHandler(LODGridChunkSystem &system, const std::string &id, Chunk &chunk);

        LODGridChunkHandler(const LODGridChunkHandler &other);

        IWorldZoneHandler *clone() const override;

        Chunk &chunk() override;

        const Chunk &getChunk() const override;

        const std::string &getID() const override;

        bool operator<(const LODGridChunkHandler &other) const;

        bool operator==(const LODGridChunkHandler &other) const;

        bool hasParent() const override;

        WorldZone getParent() const override;

        vec3d getRelativeOffset(const LODGridChunkHandler &other);
    private:
        std::string _id;
        Chunk &_chunk;
        LODGridChunkSystem &_system;

        friend class LODGridChunkSystem;
    };
}

#endif //WORLD_CHUNKSYSTEM_H
