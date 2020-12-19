#include <catch/catch.hpp>

#include <world/core.h>

using namespace world;

TEST_CASE("ItemKeys", "[collector]") {
    ItemKey key1 = ItemKeys::root("0"_k);
    ItemKey key2 = ItemKeys::root("0"_k);
    ItemKey key3 = ItemKeys::root("1"_k);

    ItemKey keyc1 = ItemKeys::child(key1, "51"_k);
    ItemKey keyc2 = ItemKeys::child(key2, "51"_k);
    ItemKey keyc3 = ItemKeys::child(key3, "51"_k);
    ItemKey keyc4 = ItemKeys::child(key1, "52"_k);

    SECTION("Keys comparison") {
        CHECK(key1 == key2);
        CHECK_FALSE(key1 < key2);
        CHECK_FALSE(key2 < key1);

        CHECK_FALSE(key3 == key1);
        CHECK((key1 < key3) != (key3 < key1));

        CHECK(keyc1 == keyc2);
        CHECK_FALSE(keyc1 < keyc2);
        CHECK_FALSE(keyc2 < keyc1);

        CHECK_FALSE(keyc3 == keyc1);
        CHECK((keyc1 < keyc3) != (keyc3 < keyc1));
        CHECK_FALSE(keyc4 == keyc1);
        CHECK((keyc1 < keyc4) != (keyc4 < keyc1));
    }

    SECTION("Concat") {
        ItemKey test = ItemKeys::child(ItemKeys::child(keyc1, "1"_k), "51"_k);
        CHECK(test == ItemKeys::concat(keyc1, keyc3));
    }

    SECTION("toString / fromString") {
        ItemKey keystr = key(ItemKeys::toString(key1));
        CHECK(key1 == keystr);
        CHECK(ItemKeys::toString(key1) == ItemKeys::toString(keystr));

        CHECK(keyc1 == key(ItemKeys::toString(keyc1)));

        CHECK_FALSE(key1.str() == key3.str());
        CHECK_FALSE(keyc1.str() == keyc3.str());
    }
}

TEST_CASE("ExplorationContext", "[collector]") {
    ExplorationContext ctx = ExplorationContext::getDefault();

    SECTION("default context") {
        CHECK(ctx.mutateKey(ItemKeys::root("a"_k)) == ItemKeys::root("a"_k));
        CHECK((ctx.getOffset() - vec3d{0, 0, 0}).norm() == Approx(0));
    }

    SECTION("context with parameters") {
        ctx.appendPrefix("a"_k);
        ctx.appendPrefix("b"_k);
        ctx.addOffset({1, 1, 1});
        ctx.addOffset({4, 5, 6});

        CHECK((ctx.getOffset() - vec3d{5, 6, 7}).norm() == Approx(0));
        auto key = ctx.mutateKey(ItemKeys::root("c"_k));
        CHECK(key == ItemKeys::child(
                         ItemKeys::child(ItemKeys::root("a"_k), "b"_k), "c"_k));
    }
}

TEST_CASE("Collector", "[collector]") {
    Collector collector;

    SECTION("adding channels") {
        CHECK_FALSE(collector.hasChannel<SceneNode>());
        CHECK_FALSE(collector.hasStorageChannel<SceneNode>());

        collector.addStorageChannel<SceneNode>();

        CHECK(collector.hasChannel<SceneNode>());
        CHECK(collector.hasStorageChannel<SceneNode>());
        CHECK_FALSE(collector.hasChannel<SceneNode, Material>());
    }

    auto &objChan = collector.addStorageChannel<SceneNode>();
    auto &meshChan = collector.addStorageChannel<Mesh>();
    auto &matChan = collector.addStorageChannel<Material>();

    REQUIRE(collector.hasStorageChannel<SceneNode, Material, Mesh>());

    SECTION("adding object") {
        auto key = ItemKeys::root("a"_k);
        Material mat01("mat01");

        CHECK_FALSE(matChan.has(key));
        CHECK_THROWS(matChan.get(key));

        matChan.put(key, mat01);
        CHECK(matChan.has(key));
        CHECK_NOTHROW(matChan.get(key));
        CHECK(matChan.get(key).getName() == key.str());

        matChan.remove(key);
        CHECK_FALSE(matChan.has(key));
        CHECK_THROWS(matChan.get(key));
    }

    SECTION("using Context") {
        ExplorationContext context;
        context.appendPrefix("a"_k);
        context.addOffset({5, 6, 7.3});

        ItemKey key = ItemKeys::root("b"_k);
        ItemKey ctxKey = ItemKeys::concat(ItemKeys::root("a"_k), key);
        SceneNode object;

        SECTION("key modification") {
            objChan.put(key, object, context);
            CHECK(objChan.has(key, context));
            CHECK(objChan.has(ctxKey));
        }

        SECTION("position modification") {
            objChan.put(key, object, context);
            CHECK((objChan.get(ctxKey).getPosition() -
                   (object.getPosition() + vec3d{5, 6, 7.3}))
                      .norm() == Approx(0));
        }
    }

    SECTION("Scene conversion") {
        ItemKey blueKey{"blue"};

        // Setup
        Mesh m;
        m.newVertex({0, 0, 0});
        m.newVertex({0, 1, 0});
        m.newVertex({0, 0, 1});
        int face[] = {0, 1, 2};
        m.newFace(face);

        Image img(1, 1, ImageType::RGBA);

        Material material("blue1");
        material.setMapKd(blueKey.str());

        SceneNode object(blueKey.str());
        object.setPosition({1, 2, 3});
        object.setMaterialID(blueKey.str());

        // Collector
        auto &imgChan = collector.addStorageChannel<Image>();

        objChan.put(blueKey, object);
        meshChan.put(blueKey, m);
        matChan.put(blueKey, material);
        imgChan.put(blueKey, img);

        // Scene conversion and tests
        Scene scene;
        collector.fillScene(scene);

        CHECK(scene.getNodes().size() == 1);
        SceneNode *sceneObj = scene.getNodes().at(0);
        CHECK(sceneObj->getPosition().length({1, 2, 3}) == Approx(0));
        CHECK(sceneObj->getMaterialID() == blueKey.str());

        CHECK(scene.materialCount() == 1);
        auto sceneMat = scene.getMaterial(blueKey.str());
        CHECK(sceneMat.getMapKd() == blueKey.str() + ".png");

        CHECK(scene.meshCount() == 1);

        // Check links
        REQUIRE(sceneMat.getName() == sceneObj->getMaterialID());
        REQUIRE(scene.hasTexture(sceneMat.getMapKd()));
    }
}
