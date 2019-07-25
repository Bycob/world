#include "TerrainOps.h"

#include <armadillo/armadillo>

namespace world {

void TerrainOps::fill(Terrain &terrain, double value) {
    terrain._array.fill(value);
}

void TerrainOps::applyOffset(Terrain &terrain, const arma::mat &offset) {
    if (offset.n_rows != terrain._array.n_rows ||
        offset.n_cols != terrain._array.n_cols) {
        throw std::runtime_error(
            "TerrainManipulator::applyOffset : bad matrix dimensions");
    }

    terrain._array += offset;
}

void TerrainOps::applyOffset(world::Terrain &terrain, double offset) {
    terrain._array += offset;
}

void TerrainOps::multiply(Terrain &terrain, const arma::mat &factor) {
    if (factor.n_rows != terrain._array.n_rows ||
        factor.n_cols != terrain._array.n_cols) {
        throw std::runtime_error(
            "TerrainManipulator::multiply : bad matrix dimensions");
    }

    terrain._array %= factor;
}

void TerrainOps::multiply(Terrain &terrain, double factor) {
    terrain._array *= factor;
}

void TerrainOps::copyNeighbours(Terrain &terrain, ITileContext &context) {
    // TODO unit test this method
    int m = terrain.getResolution() - 1;

    // corners
    auto neighbour = context.getNeighbour(-1, -1);
    if (neighbour) {
        terrain(0, 0) = (*neighbour)(m, m);
    }

    if ((neighbour = context.getNeighbour(-1, 1))) {
        terrain(0, m) = (*neighbour)(m, 0);
    }

    if ((neighbour = context.getNeighbour(1, -1))) {
        terrain(m, 0) = (*neighbour)(0, m);
    }

    if ((neighbour = context.getNeighbour(1, 1))) {
        terrain(m, m) = (*neighbour)(0, 0);
    }

    // sides
    if ((neighbour = context.getNeighbour(-1, 0))) {
        for (int i = 0; i <= m; ++i) {
            terrain(0, i) = (*neighbour)(m, i);
        }
    }

    if ((neighbour = context.getNeighbour(1, 0))) {
        for (int i = 0; i <= m; ++i) {
            terrain(m, i) = (*neighbour)(0, i);
        }
    }

    if ((neighbour = context.getNeighbour(0, -1))) {
        for (int i = 0; i <= m; ++i) {
            terrain(i, 0) = (*neighbour)(i, m);
        }
    }

    if ((neighbour = context.getNeighbour(0, 1))) {
        for (int i = 0; i <= m; ++i) {
            terrain(i, m) = (*neighbour)(i, 0);
        }
    }
}
} // namespace world
