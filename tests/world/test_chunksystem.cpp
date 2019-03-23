#include <catch/catch.hpp>

#include <world/core.h>

using namespace world;

TEST_CASE("WorldKeys", "[chunksystem]") {

    ChunkKey key{0, 0, 0, 0};

    REQUIRE(ChunkKeys::toString(key) == "00000000");
}

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

    SECTION("test uid features") {
        REQUIRE(c1a.uid().length() == sizeof(int) * 4);
        REQUIRE((c1a.uid() + c2.uid()).length() == sizeof(int) * 8);
    }

    SECTION("test fromUID") {
        REQUIRE(LODGridCoordinates::fromUID(c1a.uid()) == c1a);
        REQUIRE_THROWS(LODGridCoordinates::fromUID("ac"));
    }
}

TEST_CASE("LODGridChunkSystem", "[chunksystem]") {
    LODGridChunkSystem chunkSystem(1000);

    auto zone = chunkSystem.getZone(vec3d{-2500, -2500, -2500});

    SECTION("Test level 0 zone info") {
        REQUIRE(zone.getInfo().getDimensions().length({1000, 1000, 1000}) <
                0.001);
        REQUIRE(zone.getInfo().getParentOffset().length({-3000, -3000, -3000}) <
                0.001);
        REQUIRE(zone.getInfo().getMinResolution() == 0);
    }

    auto childZone = chunkSystem.getChildrenZones(zone)[0];

    SECTION("Test level 1 zone info") {
        CAPTURE(childZone.getInfo().getDimensions());
        REQUIRE(childZone.getInfo().getDimensions().length({250, 250, 250}) <
                0.001);
        REQUIRE(childZone.getInfo().getParentOffset().length({0, 0, 0}) <
                0.001);
        REQUIRE(childZone.getInfo().getParent());
        CAPTURE(childZone.getInfo().getAbsoluteOffset());
        REQUIRE(childZone.getInfo().getAbsoluteOffset().length(
                    {-3000, -3000, -3000}) < 0.001);
    }
}