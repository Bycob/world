
#include "PerlinTerrainGenerator.h"
#include "math/Perlin.h"
#include "assets/Image.h"


using namespace perlin;
using namespace arma;

namespace world {

PerlinTerrainGenerator::PerlinTerrainGenerator(int offset, int octaveCount,
                                               double frequency,
                                               double persistence)
        : _offset(offset), _octaveCount(octaveCount), _frequency(frequency),
          _persistence(persistence) {}

void PerlinTerrainGenerator::process(Terrain &terrain) {
    _perlin.generatePerlinNoise2D(terrain._array, _offset, _octaveCount,
                                  _frequency, _persistence);
}

void PerlinTerrainGenerator::process(Terrain &terrain,
                                     ITerrainWorkerContext &context) {
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

    _perlin.generatePerlinNoise2D(terrain._array, _offset, _octaveCount,
                                  _frequency, _persistence, false, modifier);

    context.registerCurrentState();
}

void PerlinTerrainGenerator::join(Terrain &terrain1, Terrain &terrain2,
                                  bool axisX, bool joinableSides) {
    _perlin.join(terrain1._array, terrain2._array,
                 axisX ? Direction::AXIS_X : Direction::AXIS_Y, _octaveCount,
                 _frequency, _persistence, joinableSides);
}

} // namespace world
