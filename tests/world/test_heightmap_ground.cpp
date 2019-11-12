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
            ground.observeAltitudeAt(x, y, resolution);
        }
    }

    std::shuffle(positions.begin(), positions.end(), std::mt19937());

    BENCHMARK("4 million points altitude lookup") {
        for (auto &pos : positions) {
            ground.observeAltitudeAt(pos.x, pos.y, resolution);
        }
    }
}

class TestElement : public IGridElement {
public:
    int _count;
    TestElement(int &count) {
        count++;
        _count = count;
    }

    ~TestElement() override = default;
};

TEST_CASE("GridStorage", "[terrain]") {
    // assert it does not work
    // GridStorage<vec3d> failed;

    GridStorage<TestElement> storage;
    TileCoordinates c1{{1, 2, 3}, 2};
    int count = 0;

    SECTION("set then try get") {
        TestElement *elem = nullptr;
        CHECK_FALSE(storage.tryGet(c1, &elem));
        CHECK(elem == nullptr);
        storage.set(c1, count);
        CHECK(storage.tryGet(c1, &elem));
        REQUIRE(elem != nullptr);
        CHECK(elem->_count == 1);

        // Check that element didn't get copied
        elem->_count = 2;
        elem = nullptr;
        CHECK(storage.tryGet(c1, &elem));
        REQUIRE(elem != nullptr);
        CHECK(elem->_count == 2);
    }

    SECTION("set twice") {
        storage.set(c1, count);
        auto &elem = storage.set(c1, count);
        CHECK(count == 2);
        CHECK(elem._count == 2);
    }

    SECTION("getOrCreate") {
        storage.getOrCreate(c1, count);
        auto &elem = storage.getOrCreate(c1, count);
        CHECK(count == 1);
        CHECK(elem._count == 1);
    }
}