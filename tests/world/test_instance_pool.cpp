#include <catch/catch.hpp>

#include <world/core.h>
#include <world/terrain.h>

using namespace world;

TEST_CASE("MapFilteredDistribution", "[instance pool]") {
    CHECK_THROWS(MapFilteredDistribution<SeedDistribution>(nullptr, nullptr));
}
