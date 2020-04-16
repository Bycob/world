#pragma once

#include "world/core/WorldConfig.h"

#include "WorldFile.h"

namespace world {

class World;
class Chunk;

/** This object is used to populate newly created chunks. */
class WORLDAPI_EXPORT IChunkDecorator : public ISerializable {
public:
    virtual ~IChunkDecorator() = default;

    virtual void decorate(Chunk &chunk) = 0;
};
} // namespace world