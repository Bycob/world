#pragma once

#include "core/WorldConfig.h"

#include <random>

#include "flat/FlatWorld.h"
#include "TrunkGenerator.h"
#include "TreeSkelettonGenerator.h"

namespace world {

class WORLDAPI_EXPORT SimpleTreeDecorator : public FlatWorldDecorator {
public:
    SimpleTreeDecorator(int maxTreesPerChunk = 1);

    void setModel(const Tree &model);

    void decorate(FlatWorld &world, const WorldZone &zone) override;

private:
    int _maxTreesPerChunk;
    Tree _model;

    std::mt19937 _rng;
};
} // namespace world
