#include "PerlinTerrainGenerator.h"

#include "world/math/Perlin.h"
#include "world/assets/Image.h"
#include "TerrainOps.h"

using namespace perlin;
using namespace arma;

namespace world {

WORLD_REGISTER_CHILD_CLASS(ITerrainWorker, PerlinTerrainGenerator,
                           "PerlinTerrainGenerator")

PerlinTerrainGenerator::PerlinTerrainGenerator(int octaveCount,
                                               double frequency,
                                               double persistence) {

    _perlin.setNormalize(false);

    _perlinInfo.octaves = octaveCount;
    _perlinInfo.persistence = persistence;
    _perlinInfo.repeatable = false;

    _perlinInfo.frequency = frequency;
    _perlinInfo.reference = 0;
    _perlinInfo.offsetX = 0;
    _perlinInfo.offsetY = 0;
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

void PerlinTerrainGenerator::setMaxOctaveCount(u32 maxOctaveCount) {
    _maxOctaves = maxOctaveCount;
}

void PerlinTerrainGenerator::processTerrain(Terrain &terrain) {
    _perlin.generatePerlinNoise2D(terrain._array, _perlinInfo);

    // Normalize relatively to the first lod level
    TerrainOps::multiply(terrain, 1 / _perlin.getMaxPossibleValue(_perlinInfo));
}

void PerlinTerrainGenerator::processTile(ITileContext &context) {
    processByTileCoords(context.getTile().terrain(), context);
}

void PerlinTerrainGenerator::write(WorldFile &wf) const {
    wf.addInt("maxOctaves", _maxOctaves);
    wf.addStruct("perlinInfo", _perlinInfo);
}

void PerlinTerrainGenerator::read(const WorldFile &wf) {
    wf.readUintOpt("maxOctaves", _maxOctaves);
    wf.readStruct("perlinInfo", _perlinInfo);
}

void PerlinTerrainGenerator::processByNeighbours(world::Terrain &terrain,
                                                 world::ITileContext &context) {
    TileCoordinates tc = context.getCoords();
    auto top = _storage.getopt(tc + vec2i{0, 1});
    auto bottom = _storage.getopt(tc + vec2i{0, -1});
    auto left = _storage.getopt(tc + vec2i{-1, 0});
    auto right = _storage.getopt(tc + vec2i{1, 0});
    auto topleft = _storage.getopt(tc + vec2i{-1, 1});
    auto topright = _storage.getopt(tc + vec2i{1, 1});
    auto bottomleft = _storage.getopt(tc + vec2i{-1, -1});
    auto bottomright = _storage.getopt(tc + vec2i{1, -1});

    const double eps = std::numeric_limits<double>::epsilon();

    Perlin::modifier modifier = [&](double x, double y, double val) {
        if (x < eps && y < eps && bottomleft) {
            return bottomleft->_terrain.getExactHeightAt(1, 1);
        } else if (x < eps && 1 - y < eps && topleft) {
            return topleft->_terrain.getExactHeightAt(1, 0);
        } else if (1 - x < eps && y < eps && bottomright) {
            return bottomright->_terrain.getExactHeightAt(0, 1);
        } else if (1 - x < eps && 1 - y < eps && topright) {
            return topright->_terrain.getExactHeightAt(0, 0);
        } else if (x < eps && left) {
            return left->_terrain.getExactHeightAt(1, y);
        } else if (1 - x < eps && right) {
            return right->_terrain.getExactHeightAt(0, y);
        } else if (y < eps && bottom) {
            return bottom->_terrain.getExactHeightAt(x, 1);
        } else if (1 - y < eps && top) {
            return top->_terrain.getExactHeightAt(x, 0);
        } else {
            return val;
        }
    };

    _perlin.generatePerlinNoise2D(terrain._array, _perlinInfo, modifier);

    _storage.set(tc, terrain);
}

void PerlinTerrainGenerator::processByTileCoords(Terrain &terrain,
                                                 ITileContext &context) {
    PerlinInfo localInfo = _perlinInfo;
    TileCoordinates coords = context.getCoords();
    localInfo.reference = coords._lod;
    // TODO require the frequency to be integer to avoid confusion
    vec2i tileCoords =
        vec2i(coords._pos) * static_cast<int>(localInfo.frequency);
    localInfo.offsetX = tileCoords.x;
    localInfo.offsetY = tileCoords.y;
    localInfo.octaves += localInfo.reference;
    if (_maxOctaves > 0 && localInfo.octaves > _maxOctaves)
        localInfo.octaves = _maxOctaves;

    _perlin.generatePerlinNoise2D(terrain._array, localInfo);

    // Normalize relatively to the first lod level
    TerrainOps::multiply(terrain, 1 / _perlin.getMaxPossibleValue(_perlinInfo));
}

} // namespace world
