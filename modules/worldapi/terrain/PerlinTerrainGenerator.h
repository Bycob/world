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

    void setFrequency(float frequency);

    void process(Terrain &terrain) override;

    void process(Terrain &terrain, ITerrainWorkerContext &context) override;

private:
    PerlinInfo _perlinInfo;
    Perlin _perlin;
};
} // namespace world
