#include <catch/catch.hpp>

#include <world/core.h>

using namespace world;

TEST_CASE("TileSystem", "[utilities]") {

    TileSystem ts(5, {16, 16, 0}, {1600, 1600, 0});

    SECTION("resolution to lod") {
        CHECK(ts.getLod(0.005) == 0);
        CHECK(ts.getLod(0.01) == 0);
        CHECK(ts.getLod(0.011) == 1);
        CHECK(ts.getLod(0.02) == 1);
        CHECK(ts.getLod(0.16) == 4);
        CHECK(ts.getLod(0.17) == 5);
        CHECK(ts.getLod(100) == 5);
    }

    const double eps = std::numeric_limits<double>::epsilon();

    SECTION("tile size") {
        INFO(ts.getTileSize(0));
        CHECK((ts.getTileSize(0) - vec3d{1600, 1600, 0}).norm() <= eps);
        INFO(ts.getTileSize(1));
        CHECK((ts.getTileSize(1) - vec3d{800, 800, 0}).norm() <= eps);
        INFO(ts.getTileSize(5));
        CHECK((ts.getTileSize(5) - vec3d{50, 50, 0}).norm() <= eps);
    }

    SECTION("general -> tile coordinates") {
        TileCoordinates tc = ts.getTileCoordinates(vec3d{-2000, 2000, 0}, 0);
        CHECK(tc._pos.x == -2);
        CHECK(tc._pos.y == 1);
        CHECK(tc._pos.z == 0);
        tc = ts.getTileCoordinates(vec3d{-2000, 2000, 0}, 3);
        CHECK(tc._pos.x == -10);
        CHECK(tc._pos.y == 10);
        CHECK(tc._pos.z == 0);
    }

    SECTION("get local coordinates") {
        vec3d lc = ts.getLocalCoordinates(vec3d{-2100, 2100, 0}, 2);
        CHECK(lc.x == Approx(0.75));
        CHECK(lc.y == Approx(0.25));
        CHECK(lc.z == Approx(0));
    }

    SECTION("get tile offset") {
        vec3d to = ts.getTileOffset({{-3, 1, 0}, 2});
        CHECK(to.x == Approx(-1200));
        CHECK(to.y == Approx(400));
        CHECK(to.z == Approx(0));
    }

    SECTION("tile -> global") {
        vec3d gc =
            ts.getGlobalCoordinates({{-3, 1, 0}, 2}, {0.0525, 0.0325, 0});
        CHECK(gc.x == Approx(-1179));
        CHECK(gc.y == Approx(413));
        CHECK(gc.z == Approx(0));
    }
}

TEST_CASE("Test StringOps.h", "[utilities]") {

    SECTION("split") {
        std::string fullstr("I,,am,splitted,by,comas,");
        REQUIRE(split(fullstr, ',') ==
                std::vector<std::string>{"I", "", "am", "splitted", "by",
                                         "comas", ""});
        REQUIRE(split(fullstr, ',', true) ==
                std::vector<std::string>{"I", "am", "splitted", "by", "comas"});
        REQUIRE(split("a,,b", ',') == std::vector<std::string>{"a", "", "b"});
    }

    SECTION("trimSpaces") {
        REQUIRE(trimSpaces("  \t trim spaces ! \t\t") == "trim spaces !");
        REQUIRE(trimSpaces("\n \ntrim spaces\n") == "\n \ntrim spaces\n");
    }

    SECTION("startsWith") {
        REQUIRE(startsWith("bbaabba", "bba"));
        REQUIRE_FALSE(startsWith("babaabba", "bba"));
        REQUIRE_FALSE(startsWith("baba", "bababa"));
        REQUIRE(startsWith("a", ""));
        REQUIRE(startsWith(".png", ".png"));
    }

    SECTION("endsWith") {
        REQUIRE(endsWith("extension.png.png", ".png"));
        REQUIRE_FALSE(endsWith("extension.png", ".png.png"));
        REQUIRE_FALSE(endsWith(".png", ".png.png"));
        REQUIRE(endsWith("a", ""));
        REQUIRE(endsWith(".png", ".png"));
    }
}
