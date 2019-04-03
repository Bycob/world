#ifndef WORLD_ICHUNKSYSTEM_H
#define WORLD_ICHUNKSYSTEM_H

#include "world/core/WorldConfig.h"

#include "WorldKeys.h"
#include "Chunk.h"

namespace world {
class WORLDAPI_EXPORT IChunkSystem {
public:
    virtual ~IChunkSystem() = default;

    /** Get the chunk at the specified position and resolution */
    virtual Chunk &getChunk(const vec3d &position, double resolution) = 0;
};
} // namespace world
#endif // WORLD_ICHUNKSYSTEM_H
