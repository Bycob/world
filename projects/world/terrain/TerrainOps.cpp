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

void TerrainOps::copyNeighbours(Terrain &terrain, const TileCoordinates &coords,
                                const TerrainGrid &storage) {
    // TODO unit test this method
    int m = terrain.getResolution() - 1;

    // corners
    TerrainElement *neighbour;
    if (storage.tryGet(coords + vec2i{-1, -1}, &neighbour)) {
        terrain(0, 0) = neighbour->_terrain(m, m);
    }

    if (storage.tryGet(coords + vec2i{-1, 1}, &neighbour)) {
        terrain(0, m) = neighbour->_terrain(m, 0);
    }

    if (storage.tryGet(coords + vec2i{1, -1}, &neighbour)) {
        terrain(m, 0) = neighbour->_terrain(0, m);
    }

    if (storage.tryGet(coords + vec2i{1, 1}, &neighbour)) {
        terrain(m, m) = neighbour->_terrain(0, 0);
    }

    // sides
    if (storage.tryGet(coords + vec2i{-1, 0}, &neighbour)) {
        for (int i = 0; i <= m; ++i) {
            terrain(0, i) = neighbour->_terrain(m, i);
        }
    }

    if (storage.tryGet(coords + vec2i{1, 0}, &neighbour)) {
        for (int i = 0; i <= m; ++i) {
            terrain(m, i) = neighbour->_terrain(0, i);
        }
    }

    if (storage.tryGet(coords + vec2i{0, -1}, &neighbour)) {
        for (int i = 0; i <= m; ++i) {
            terrain(i, 0) = neighbour->_terrain(i, m);
        }
    }

    if (storage.tryGet(coords + vec2i{0, 1}, &neighbour)) {
        for (int i = 0; i <= m; ++i) {
            terrain(i, m) = neighbour->_terrain(i, 0);
        }
    }
}

vec2d TerrainOps::computeZBounds(const Terrain &terrain) {
    vec2d bounds{std::numeric_limits<double>::max(),
                 std::numeric_limits<double>::min()};

    for (int y = 0; y < terrain.getResolution(); ++y) {
        for (int x = 0; x < terrain.getResolution(); ++x) {
            double val = terrain(x, y);

            if (bounds.x > val) {
                bounds.x = val;
            }

            if (bounds.y < val) {
                bounds.y = val;
            }
        }
    }

    return bounds;
}
} // namespace world
