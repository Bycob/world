#pragma once

#include "world/core/WorldConfig.h"

#include <random>

#include "world/flat/FlatWorld.h"
#include "world/core/IChunkDecorator.h"
#include "Tree.h"

namespace world {

class WORLDAPI_EXPORT SimpleTreeDecorator : public IChunkDecorator {
    WORLD_WRITE_SUBCLASS_METHOD
public:
    SimpleTreeDecorator(int maxTreesPerChunk = 1);

    void setModel(const Tree &model);

    void decorate(Chunk &chunk, const ExplorationContext &ctx) override;

private:
    int _maxTreesPerChunk;
    Tree _model;

    std::mt19937 _rng;
};
} // namespace world
