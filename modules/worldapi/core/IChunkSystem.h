#ifndef WORLD_ICHUNKSYSTEM_H
#define WORLD_ICHUNKSYSTEM_H

#include "core/WorldConfig.h"

#include "WorldKeys.h"
#include "WorldZone.h"

namespace world {
class IChunkSystem {
public:
    virtual ~IChunkSystem() = default;

    /**
     * Get the chunk at the given position. If multiple chunk with
     * different levels of detail are found, the one with the highest
     * LOD is returned. If no chunk is found, a chunk is created.
     * @param position - The absolute position in the world of the chunk
     * we want to get.
     * @return A QueryResult containing the chunk and a boolean indicating
     * if it was newly created.
     */
    virtual WorldZone getZone(const vec3d &position) = 0;

    virtual std::vector<WorldZone> getNeighbourZones(const WorldZone &zone) = 0;

    virtual std::vector<WorldZone> getChildrenZones(const WorldZone &zone) = 0;

    virtual Chunk &getChunk(const WorldZone &zone) = 0;

    virtual void collectZone(const WorldZone &zone, ICollector &collector,
                             IResolutionModel &resolutionModel) = 0;
};
} // namespace world
#endif // WORLD_ICHUNKSYSTEM_H
