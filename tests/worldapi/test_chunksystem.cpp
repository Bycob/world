#include <catch/catch.hpp>

#include <worldcore.h>

using namespace world;

TEST_CASE("LODGridCoordinates", "[chunksystem]") {
    LODGridCoordinates none = LODGridCoordinates::NONE;
    LODGridCoordinates c1a(0, 0, 0, 0);
    LODGridCoordinates c1b(0, 0, 0, 0);
    LODGridCoordinates c2(0, 0, 0, 1);
    LODGridCoordinates c3(0, 0, 1, 0);

    SECTION("test operator==") {
        REQUIRE(c1a == c1b);
        REQUIRE_FALSE(c1a == c2);
        REQUIRE_FALSE(c1a == c3);
        REQUIRE_FALSE(c2 == c3);
    }

    SECTION("test uid") {
        REQUIRE(c1a.uid() == c1b.uid());
        REQUIRE(c1a.uid() != c2.uid());
        REQUIRE(c1a.uid() != c3.uid());
        REQUIRE(c2.uid() != c3.uid());
    }
}

TEST_CASE("LODGridChunkSystem", "[chunksystem]") {

}