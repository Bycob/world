#pragma once

#include "core/WorldConfig.h"

#include <memory>
#include <map>
#include <utility>

#include "core/WorldTypes.h"
#include "ITerrainWorker.h"
#include "math/Perlin.h"
#include "Terrain.h"

namespace world {

class WORLDAPI_EXPORT PerlinTerrainGenerator : public ITerrainWorker {
public:
    PerlinTerrainGenerator(int offset = 0, int octaveCount = 5,
                           double frequency = 8, double persistence = 0.5);

    void setFrequency(float frequency) { _frequency = frequency; }

    void process(Terrain &terrain) override;

    void process(Terrain &terrain, ITerrainWorkerContext &context) override;

    void join(Terrain &terrain1, Terrain &terrain2, bool axisX,
              bool joinableSides);

private:
    Perlin _perlin;

    int _offset;
    int _octaveCount;
    double _frequency;
    double _persistence;
    double _subdivNoiseRatio = 0.1;
};
} // namespace world
