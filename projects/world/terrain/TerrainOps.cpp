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
} // namespace world
