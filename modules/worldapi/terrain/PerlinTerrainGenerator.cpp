#include "PerlinTerrainGenerator.h"

#include "math/Perlin.h"
#include "assets/Image.h"
#include "TerrainOps.h"

using namespace perlin;
using namespace arma;

namespace world {

PerlinTerrainGenerator::PerlinTerrainGenerator(int octaveCount,
                                               double frequency,
                                               double persistence) {

    _perlin.setNormalize(false);

    _perlinInfo.octaves = octaveCount;
    _perlinInfo.persistence = persistence;
    _perlinInfo.repeatable = false;

    _perlinInfo.frequency = frequency;
}

void PerlinTerrainGenerator::setFrequency(double frequency) {
    _perlinInfo.frequency = frequency;
}

void PerlinTerrainGenerator::setPersistence(double persistence) {
    _perlinInfo.persistence = persistence;
}

void PerlinTerrainGenerator::setOctaveCount(int octaveCount) {
    _perlinInfo.octaves = octaveCount;
}

void PerlinTerrainGenerator::process(Terrain &terrain) {
    _perlin.generatePerlinNoise2D(terrain._array, _perlinInfo);
}

void PerlinTerrainGenerator::process(Terrain &terrain,
                                     ITerrainWorkerContext &context) {
    processByTileCoords(terrain, context);
}

void PerlinTerrainGenerator::processByNeighbours(
    world::Terrain &terrain, world::ITerrainWorkerContext &context) {
    auto top = context.getNeighbour(0, 1);
    auto bottom = context.getNeighbour(0, -1);
    auto left = context.getNeighbour(-1, 0);
    auto right = context.getNeighbour(1, 0);
    auto topleft = context.getNeighbour(-1, 1);
    auto topright = context.getNeighbour(1, 1);
    auto bottomleft = context.getNeighbour(-1, -1);
    auto bottomright = context.getNeighbour(1, -1);

    const double eps = std::numeric_limits<double>::epsilon();

    Perlin::modifier modifier = [&](double x, double y, double val) {
        if (x < eps && y < eps && bottomleft) {
            return bottomleft->getExactHeightAt(1, 1);
        } else if (x < eps && 1 - y < eps && topleft) {
            return topleft->getExactHeightAt(1, 0);
        } else if (1 - x < eps && y < eps && bottomright) {
            return bottomright->getExactHeightAt(0, 1);
        } else if (1 - x < eps && 1 - y < eps && topright) {
            return topright->getExactHeightAt(0, 0);
        } else if (x < eps && left) {
            return left->getExactHeightAt(1, y);
        } else if (1 - x < eps && right) {
            return right->getExactHeightAt(0, y);
        } else if (y < eps && bottom) {
            return bottom->getExactHeightAt(x, 1);
        } else if (1 - y < eps && top) {
            return top->getExactHeightAt(x, 0);
        } else {
            return val;
        }
    };

    _perlin.generatePerlinNoise2D(terrain._array, _perlinInfo, modifier);

    context.registerCurrentState();
}

void PerlinTerrainGenerator::processByTileCoords(
    Terrain &terrain, ITerrainWorkerContext &context) {
    PerlinInfo localInfo = _perlinInfo;
    // TODO explicitly get the reference from Ground data instead of hard-coding
    localInfo.reference = context.getParentCount() * 2;
    // TODO require the frequency to be integer to avoid confusion
    vec2i tileCoords =
        context.getTileCoords() * static_cast<int>(localInfo.frequency);
    localInfo.offsetX = tileCoords.x;
    localInfo.offsetY = tileCoords.y;
    localInfo.octaves += localInfo.reference;

    _perlin.generatePerlinNoise2D(terrain._array, localInfo);

    // Normalize relatively to the first lod level
    TerrainOps::multiply(terrain, 1 / _perlin.getMaxPossibleValue(_perlinInfo));
}

} // namespace world
