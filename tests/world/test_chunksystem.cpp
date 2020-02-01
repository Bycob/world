#include <catch/catch.hpp>

#include <world/core.h>

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

        CHECK_FALSE(none == c1a);
        CHECK_FALSE(none == c3);
    }

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

    SECTION("test getLastOfKey") {
        REQUIRE(LODGridCoordinates::getLastOfKey(c1a.toKey()) == c1a);
        REQUIRE_THROWS(LODGridCoordinates::getLastOfKey("ac"));
    }

    SECTION("test parenting") {
        NodeKey parent = c2.toKey();
        NodeKey key = c3.toKey(parent);
        CHECK(key.length() == sizeof(int) * 8);

        LODGridCoordinates c3a = LODGridCoordinates::getLastOfKey(key);
        CHECK(c3a == c3);

        NodeKey parentA = LODGridCoordinates::getParent(key);
        CHECK(parent == parentA);

        LODGridCoordinates c2a = LODGridCoordinates::getLastOfKey(parentA);
        CHECK(c2a == c2);

        NodeKey parentB = LODGridCoordinates::getParent(parentA);
        CHECK(parentB == NodeKeys::none());
        CHECK(LODGridCoordinates::getLastOfKey(parentB) == none);
        CHECK_THROWS(LODGridCoordinates::getParent(parentB));
    }
}

class ExplorationSpy;

class SnitchNode : public WorldNode {
public:
    ExplorationSpy &_spy;


    SnitchNode(ExplorationSpy &spy) : _spy(spy) {}

    void collect(ICollector &collector, const IResolutionModel &model,
                 const ExplorationContext &ctx) override;
};

class ExplorationSpy : public IChunkDecorator {
public:
    bool _verbose = false;
    int _chunkCounter = 0;
    std::map<NodeKey, vec3d> _positions;

    void decorate(Chunk &chunk) override {
        if (_verbose) {
            std::cout << "Exploration of chunk n°" << _chunkCounter
                      << std::endl;
            std::cout << "\tDimensions: " << chunk.getSize()
                      << ", position: " << chunk.getPosition3D() << std::endl;
            std::cout << "\tResolution: min " << chunk.getMinResolution()
                      << ", max " << chunk.getMaxResolution() << std::endl;
        }

        SnitchNode &node = chunk.addChild<SnitchNode>(*this);
        node.setPosition3D(vec3d{0, 0, 0});

        ++_chunkCounter;
    }
};

void SnitchNode::collect(ICollector &collector, const IResolutionModel &model,
                         const ExplorationContext &ctx) {
    vec3d offset = ctx.getOffset();
    ItemKey itemKey = ctx.mutateKey(ItemKeys::defaultKey());
    NodeKey nodeKey = ItemKeys::getLastNode(ItemKeys::getParent(itemKey));
    _spy._positions[nodeKey] = offset;
}

TEST_CASE("LODGridChunkSystem", "[chunksystem]") {
    LODGridChunkSystem chunkSystem(1000, 6, 0.5);
    ExplorationSpy &spy = chunkSystem.addDecorator<ExplorationSpy>();

    SECTION("TileSystem config") {
        const TileSystem &ts = chunkSystem.getTileSystem();

        std::stringstream resolutions;

        for (int i = 0; i <= ts._maxLod; ++i) {
            resolutions << ts.getMinResolution(i) << " "
                        << ts.getMaxResolution(i) << std::endl;
        }
        INFO(resolutions.str());

        CHECK(ts.getMaxResolution(0) == Approx(0.5));
    }

    SECTION("Collecting") {
        FirstPersonView fpsView;
        Collector collector;

        chunkSystem.collect(collector, fpsView);

        bool success = true;
        std::stringstream errors;

        for (auto &entry : spy._positions) {
            vec3d goodOffset = chunkSystem.getOffset({entry.first});
            vec3d realOffset = entry.second;

            if (goodOffset.length(realOffset) != Approx(0)) {
                success = false;
                errors << realOffset << " should be " << goodOffset
                       << std::endl;
            }
        }

        INFO(errors.str());
        CHECK(success);
    }
}