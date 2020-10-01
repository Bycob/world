#include <catch/catch.hpp>

#include <world/core.h>
#include <world/terrain.h>

using namespace world;

class DummyTileContext : public ITileContext {
public:
    TerrainTile &terrain;

    DummyTileContext(TerrainTile &terrain) : terrain(terrain) {}

    TerrainTile &getTile() const override { return terrain; }

    TileCoordinates getCoords() const override { return {{}, 1}; }

    TileCoordinates getParentCoords() const override { return {}; }

    const ExplorationContext &getExplorationContext() const override {
        return ExplorationContext::getDefault();
    }
};

TEST_CASE("Test diamond square terrain generation", "[diamond_square]") {
    TerrainTile tile({}, 129);
    Terrain &terrain = tile._terrain;
    TerrainOps::fill(terrain, 0);

    double accu = 0;
    for (int y = 0; y < terrain.getResolution(); ++y) {
        for (int x = 0; x < terrain.getResolution(); ++x) {
            accu += abs(terrain(x, y));
        }
    }
    REQUIRE(accu == Approx(0));

    DiamondSquareTerrain diasqua;

    SECTION("Generation from scratch") {
        diasqua.processTerrain(terrain);

        bool no_nulls = true;
        for (int y = 0; y < terrain.getResolution(); ++y) {
            for (int x = 0; x < terrain.getResolution(); ++x) {
                if (abs(terrain(x, y)) <
                    std::numeric_limits<double>::epsilon()) {

                    std::cout << vec2i{x, y} << " is " << terrain(x, y)
                              << std::endl;
                    no_nulls = false;
                }
            }
        }
        CHECK(no_nulls);
    }

    SECTION("Generation from parent") {
        // Create parent
        Terrain parent(65);
        TerrainOps::fill(parent, 2);
        diasqua.getStorage()->set({}, parent);

        DummyTileContext ctx(tile);

        SECTION("with jitter") {
            diasqua.processTile(ctx);

            accu = 0;
            double accu_parent = 0;
            for (int y = 0; y < parent.getResolution(); ++y) {
                for (int x = 0; x < parent.getResolution(); ++x) {
                    accu_parent += abs(2 - parent(x, y));
                    accu += abs(parent(x, y) - terrain(x * 2, y * 2));
                }
            }
            REQUIRE(accu_parent == Approx(0));
            CHECK(accu == Approx(0));
        }

        SECTION("without jitter") {
            DiamondSquareTerrain woJitter(0);
            woJitter.getStorage()->set({}, parent);
            woJitter.processTile(ctx);

            accu = 0;
            for (int y = 0; y < terrain.getResolution(); ++y) {
                for (int x = 0; x < terrain.getResolution(); ++x) {
                    accu += abs(2 - terrain(x, y));
                }
            }
            CHECK(accu == Approx(0));
        }
    }

    // TODO test errors when terrain has an invalid size
}