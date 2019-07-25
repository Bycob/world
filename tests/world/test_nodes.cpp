#include <catch/catch.hpp>

#include <world/core.h>

using namespace world;

TEST_CASE("NodeKeys", "[nodes]") {
    NodeKey key{0, 0, 0, 0};
    REQUIRE(NodeKeys::toString(key) == "00000000");
}


class TestNode : public WorldNode {
public:
    bool _collected = false;
    vec3d _offset;
    ItemKey _keyPrefix;


    void collect(ICollector &collector, const IResolutionModel &resolutionModel,
                 const ExplorationContext &ctx) override {
        _collected = true;
        _keyPrefix = ctx.mutateKey(ItemKeys::defaultKey());
        _offset = ctx.getOffset();
    }
};

TEST_CASE("WorldNode", "[nodes]") {
    WorldNode node1;
    TestNode &node2 = node1.addChild<TestNode>();
    TestNode &node3 = node1.addChild<TestNode>();

    Collector collector;

    node1.setPosition3D({1., 2., 3.});
    node2.setPosition3D({6., 4., 2.});
    node3.setPosition3D({18., 23., 85.});

    SECTION("Child gets collected") {
        node1.collectAll(collector, 1);
        CHECK(node2._collected);
        CHECK(node3._collected);
    }

    SECTION("Child is contextualized") {
        node1.collectAll(collector, 1);
        CHECK(node2._keyPrefix != ItemKeys::defaultKey());
        CHECK((node2._offset - vec3d{6., 4., 2.}).norm() == Approx(0.0));
        CHECK(node3._keyPrefix != ItemKeys::defaultKey());
        CHECK((node3._offset - vec3d{18., 23., 85.}).norm() == Approx(0.0));
        CHECK(node2._keyPrefix != node3._keyPrefix);
    }
}