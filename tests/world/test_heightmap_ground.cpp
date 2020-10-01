#include <catch/catch.hpp>

#include <random>

#include <world/core.h>
#include <world/terrain.h>

using namespace world;

TEST_CASE("HeightmapGround - observeAltitudeAt benchmark",
          "[terrain][!benchmark]") {
    HeightmapGround ground(6000);
    ground.setDefaultWorkerSet();

    double resolution = 0.01;
    std::vector<vec2d> positions;

    for (int x = -10000; x < 10000; x += 10) {
        for (int y = -10000; y < 10000; y += 10) {
            positions.emplace_back(x, y);
            ground.observeAltitudeAt(x, y, resolution,
                                     ExplorationContext::getDefault());
        }
    }

    std::shuffle(positions.begin(), positions.end(), std::mt19937());

    BENCHMARK("4 million points altitude lookup") {
        for (auto &pos : positions) {
            ground.observeAltitudeAt(pos.x, pos.y, resolution,
                                     ExplorationContext::getDefault());
        }
    }
}
