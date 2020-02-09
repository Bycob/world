#include <catch/catch.hpp>

#include <world/core.h>

using namespace world;

TEST_CASE("TileSystem", "[utilities]") {

    SECTION("TileCoordinates") {
        TileCoordinates c1a(0, 0, 0, 0);
        TileCoordinates c1b(0, 0, 0, 0);
        TileCoordinates c2(0, 0, 0, 1);
        TileCoordinates c3(0, 0, 1, 0);

        SECTION("test operator==") {
            REQUIRE(c1a == c1b);
            REQUIRE_FALSE(c1a == c2);
            REQUIRE_FALSE(c1a == c3);
            REQUIRE_FALSE(c2 == c3);
        }

        // TODO Test operator<

        SECTION("test toKey") {
            REQUIRE(c1a.toKey() == c1b.toKey());
            REQUIRE(c1a.toKey() != c2.toKey());
            REQUIRE(c1a.toKey() != c3.toKey());
            REQUIRE(c2.toKey() != c3.toKey());
        }

        SECTION("test toKey features") {
            REQUIRE(c1a.toKey().length() == sizeof(int) * 4);
            REQUIRE((c1a.toKey() + c2.toKey()).length() == sizeof(int) * 8);
        }
    }

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

class TestElement : public IGridElement {
public:
    int _count;
    TestElement(int &count) {
        count++;
        _count = count;
    }

    ~TestElement() override = default;
};

class DummyGridStorage : public GridStorageBase {
public:
    std::set<TileCoordinates> _tcs;
    GridStorageReducer *_reducer = nullptr;

    void add(const TileCoordinates &coords) {
        if (_reducer != nullptr) {
            _reducer->registerAccess(coords);
        }
        _tcs.insert(coords);
    }

    void remove(const TileCoordinates &coords) override { _tcs.erase(coords); }

    bool has(const TileCoordinates &coords) const override {
        return _tcs.find(coords) != _tcs.end();
    }
};

TEST_CASE("GridStorage", "[utilities]") {
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

    SECTION("GridStorageReducer") {
        TileSystem ts{1, {1}, {1}};

        DummyGridStorage storage;
        GridStorageReducer reducer(ts, 3);

        storage._reducer = &reducer;
        reducer.registerStorage(&storage);

        // p = parent, c = child, r = removed
        TileCoordinates p1 = {{0}, 0}, p1c1r = {{0}, 1}, p2r = {{1}, 0},
                        p2cr = {{2}, 1}, p3 = {{-1}, 0}, p3cr = {{-1}, 1},
                        p1c2 = {{1}, 1};

        // Check for mistake in coordinate choice
        REQUIRE(ts.getParentTileCoordinates(p1c1r) == p1);
        REQUIRE(ts.getParentTileCoordinates(p1c2) == p1);
        REQUIRE(ts.getParentTileCoordinates(p2cr) == p2r);
        REQUIRE(ts.getParentTileCoordinates(p3cr) == p3);

        // Actual testing
        storage.add(p1);
        storage.add(p1c1r);
        storage.add(p2r);
        storage.add(p2cr);
        storage.add(p3);
        storage.add(p3cr);
        storage.add(p1c2);

        reducer.reduceStorage();

        CHECK(storage._tcs.find(p1) != storage._tcs.end());
        CHECK(storage._tcs.find(p1c1r) == storage._tcs.end());
        CHECK(storage._tcs.find(p2r) == storage._tcs.end());
        CHECK(storage._tcs.find(p2cr) == storage._tcs.end());
        CHECK(storage._tcs.find(p3) != storage._tcs.end());
        CHECK(storage._tcs.find(p3cr) == storage._tcs.end());
        CHECK(storage._tcs.find(p1c2) != storage._tcs.end());
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
