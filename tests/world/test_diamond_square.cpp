#include <catch/catch.hpp>

#include <world/core.h>
#include <world/terrain.h>

using namespace world;

class DummyTileContext : public ITileContext {
public:
    Terrain &terrain;
    Terrain &parent;

    DummyTileContext(Terrain &terrain, Terrain &parent)
            : terrain(terrain), parent(parent) {}

    Terrain &getTerrain() const override { return terrain; }

    Image &getTexture() const override { return terrain.getTexture(); }

    optional<const Terrain &> getNeighbour(int x, int y) const override {
        return nullopt;
    }

    optional<const Terrain &> getParent() const override { return parent; }

    int getParentCount() const override { return 1; }

    vec2i getTileCoords() const override { return {0, 0}; }

    void registerCurrentState() override {}
};

TEST_CASE("Test diamond square terrain generation", "[diamond_square]") {
    Terrain terrain(129);
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
                    no_nulls = false;
                }
            }
        }
        CHECK(no_nulls);
    }

    SECTION("Generation from parent") {
        Terrain parent(65);
        TerrainOps::fill(parent, 2);
        DummyTileContext ctx(terrain, parent);

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

    // TODO test errors when
}