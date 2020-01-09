#include <catch/catch.hpp>

#include <world/core.h>
#include <world/terrain.h>

using namespace world;

TEST_CASE("Templates", "[instance pool]") {
    Template tmplate;

    CHECK(tmplate.getAt(0) == nullptr);
    CHECK(tmplate.getAt(12) == nullptr);

    SECTION("Add node without resolution") {
        tmplate.insert(SceneNode("dudu"));
        REQUIRE(tmplate.getAt(12) != nullptr);
        REQUIRE(tmplate.getAt(0) != nullptr);

        auto *item = tmplate.getAt(0);
        REQUIRE(item->_nodes.size() == 1);
        CHECK(item->_nodes[0].getMeshID() == "dudu");
        CHECK(item->_minRes == Approx(0));
    }

    SECTION("Create template with a scene node") {
        Template tmplate2(SceneNode("dudu"));
        REQUIRE(tmplate2.getAt(0) != nullptr);

        auto *item = tmplate2.getAt(0);
        CHECK(item->_nodes.size() == 1);
        CHECK(item->_minRes == Approx(0));
    }

    SECTION("Add item at a given resolution") {
        tmplate.insert(10, SceneNode("dada"));
        tmplate.insert(15, SceneNode("dodo"));

        CHECK(tmplate.getAt(0) == nullptr);
        CHECK(tmplate.getAt(1) == nullptr);
        REQUIRE(tmplate.getAt(10) != nullptr);
        REQUIRE(tmplate.getAt(11) != nullptr);
        REQUIRE(tmplate.getAt(15) != nullptr);
        REQUIRE(tmplate.getAt(20) != nullptr);

        CHECK(tmplate.getAt(10)->_minRes == Approx(10));
        CHECK(tmplate.getAt(11)->_minRes == Approx(10));
        CHECK(tmplate.getAt(15)->_minRes == Approx(15));
        CHECK(tmplate.getAt(20)->_minRes == Approx(15));

        CHECK(tmplate.getAt(11)->_nodes.at(0).getMeshID() == "dada");
        CHECK(tmplate.getAt(15)->_nodes.at(0).getMeshID() == "dodo");
    }
}

TEST_CASE("MapFilteredDistribution", "[instance pool]") {
    CHECK_THROWS(MapFilteredDistribution<SeedDistribution>(nullptr, nullptr));
}
