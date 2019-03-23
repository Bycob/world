#include <catch/catch.hpp>

#include <world/core.h>
#include <world/terrain.h>

using namespace world;

TEST_CASE("Terrain - getExactHeightAt", "[terrain]") {
    Terrain terrain(2);
    terrain(0, 0) = 1;
    terrain(0, 1) = 0;
    terrain(1, 0) = 0;
    terrain(1, 1) = 1;

    SECTION("getExactHeightAt trivial coordinates") {
        REQUIRE(terrain.getExactHeightAt(0, 0) == Approx(1));
        REQUIRE(terrain.getExactHeightAt(0, 1) == Approx(0));
        REQUIRE(terrain.getExactHeightAt(1, 0) == Approx(0));
        REQUIRE(terrain.getExactHeightAt(1, 1) == Approx(1));
    }

    SECTION("getExactHeightAt non trivial coordinates") {
        REQUIRE(terrain.getExactHeightAt(0.5, 0) == Approx(0.5));
        REQUIRE(terrain.getExactHeightAt(0, 0.5) == Approx(0.5));
        REQUIRE(terrain.getExactHeightAt(0.5, 1) == Approx(0.5));
        REQUIRE(terrain.getExactHeightAt(1, 0.5) == Approx(0.5));
    }

    terrain(0, 0) = 0.1;
    terrain(0, 1) = 0.4;
    terrain(1, 0) = 0.5;
    terrain(1, 1) = 0.7;

    SECTION("getExactHeightAt trivial coordinates, non trivial heights") {
        REQUIRE(terrain.getExactHeightAt(0, 0) == Approx(0.1));
        REQUIRE(terrain.getExactHeightAt(0, 1) == Approx(0.4));
        REQUIRE(terrain.getExactHeightAt(1, 0) == Approx(0.5));
        REQUIRE(terrain.getExactHeightAt(1, 1) == Approx(0.7));
    }

    SECTION("getExactHeightAt non trivial coordinates, non trivial heights") {

        std::stringstream str;
        for (double y = 1; y >= -0.02; y -= 0.05) {
            for (double x = 0; x <= 1.01; x += 0.05) {
                str << terrain.getExactHeightAt(x, y) << " ";
            }
            str << std::endl;
        }
        INFO(str.str());

        REQUIRE(terrain.getExactHeightAt(0.5, 0) == Approx(0.3));
        REQUIRE(terrain.getExactHeightAt(0, 0.5) == Approx(0.25));
        REQUIRE(terrain.getExactHeightAt(0.5, 1) == Approx(0.55));
        REQUIRE(terrain.getExactHeightAt(1, 0.5) == Approx(0.6));
    }
}

TEST_CASE("Terrain - Mesh generation benchmark", "[terrain][!benchmark]") {
    Terrain terrain(129);

    BENCHMARK("Create a mesh") {
        Mesh *mesh = terrain.createMesh();
        delete mesh;
    }
}