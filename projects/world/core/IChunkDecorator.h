#pragma once

#include "world/core/WorldConfig.h"

namespace world {

    class World;
    class Chunk;

/** This object is used to populate newly created chunks. */
class IChunkDecorator {
public:
    virtual ~IChunkDecorator() = default;

    virtual void decorate(Chunk &chunk) = 0;
};
} // namespace world