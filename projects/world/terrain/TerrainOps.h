#pragma once

#include "world/core/WorldConfig.h"

#include "Terrain.h"

namespace world {

class WORLDAPI_EXPORT TerrainOps {
public:
    static void applyOffset(Terrain &terrain, const arma::mat &offset);

    static void applyOffset(Terrain &terrain, double offset);

    static void multiply(Terrain &terrain, const arma::mat &factor);

    static void multiply(Terrain &terrain, double factor);
};
} // namespace world
