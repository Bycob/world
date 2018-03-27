#ifndef WORLD_ICHUNKSYSTEM_H
#define WORLD_ICHUNKSYSTEM_H

#include <worldapi/worldapidef.h>

#include "WorldZone.h"

namespace world {
    class IChunkSystem {
    public:
        struct QueryResult {
            WorldZone _zone;
            bool _created;
        };

        /**
         * Get the chunk at the given position. If multiple chunk with
         * different levels of detail are found, the one with the highest
         * LOD is returned. If no chunk is found, a chunk is created.
         * @param position - The absolute position in the world of the chunk
         * we want to get.
         * @return A QueryResult containing the chunk and a boolean indicating
         * if it was newly created.
         */
        virtual QueryResult getChunk(const vec3d &position) = 0;

        virtual QueryResult getNeighbourChunk(const WorldZone &zone, const vec3i &direction) = 0;

        virtual std::vector<QueryResult> getChildren(const WorldZone &zone) = 0;
    };
}
#endif //WORLD_ICHUNKSYSTEM_H
