#pragma once

#include "world/core/WorldConfig.h"

#include <random>

#include "world/flat/FlatWorld.h"
#include "world/core/IChunkDecorator.h"
#include "Tree.h"

namespace world {

class WORLDAPI_EXPORT SimpleTreeDecorator : public IChunkDecorator {
public:
    SimpleTreeDecorator(FlatWorld *flatWorld, int maxTreesPerChunk = 1);

    void setModel(const Tree &model);

    void decorate(Chunk &chunk) override;

private:
    int _maxTreesPerChunk;
    Tree _model;
    FlatWorld *_flatWorld;

    std::mt19937 _rng;
};
} // namespace world
