#pragma once

#include "world/core/WorldConfig.h"

#include "WorldFile.h"
#include "ExplorationContext.h"

namespace world {

class World;
class Chunk;

/** This object is used to populate newly created chunks. */
class WORLDAPI_EXPORT IChunkDecorator : public ISerializable {
public:
    ~IChunkDecorator() override = default;

    virtual void decorate(
        Chunk &chunk,
        const ExplorationContext &ctx = ExplorationContext::getDefault()) = 0;
};
} // namespace world