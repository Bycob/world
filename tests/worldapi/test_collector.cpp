#include <catch/catch.hpp>

#include <worldcore.h>

using namespace world;

using ItemKey = ICollector::ItemKey;
using ItemKeys = ICollector::ItemKeys;

TEST_CASE("ItemKeys", "[collector]") {

    SECTION("Part keys") {
        ItemKey key1 = ItemKeys::inObject(0);
        ItemKey key2 = ItemKeys::inObject(0);
        REQUIRE(key1 == key2);
        REQUIRE_FALSE(key1 < key2);
        REQUIRE_FALSE(key2 < key1);
    }
}

class TestCollector : public ICollector {
public:
    void addItem(const ItemKey &key, const Object3D &object) override {
        _lastAdded = key;
        _lastAddedPosition = object.getPosition();
    };

    bool hasItem(const ItemKey &key) const override {
        return _lastAdded == key;
    };

    void removeItem(const ItemKey &key) override {
        _lastRemoved = key;
    };

    void addMaterial(const ItemKey &key, const Material &material) override {
        _lastMaterialAdded = key;
    };

    ItemKey _lastAdded;
    vec3d _lastAddedPosition;
    ItemKey _lastRemoved;
    ItemKey _lastMaterialAdded;
};

TEST_CASE("CollectorContextWrap", "[collector]") {
    TestCollector collector;
    CollectorContextWrap wcollector(collector);

    ChunkKey chunkKey("001122");
    ObjectKey objKey(2);
    vec3d offset{1, 1, 1};

    SECTION("default context wrap just pass arguments") {
        Object3D obj;
        obj.setPosition(offset);
        wcollector.addItem(ItemKeys::inWorld(chunkKey, objKey, 1), obj);
        REQUIRE(collector._lastAdded == ItemKeys::inWorld(chunkKey, objKey, 1));
        REQUIRE((collector._lastAddedPosition - offset).norm() == Approx(0));
    }

    wcollector.setCurrentChunk(chunkKey);
    wcollector.setCurrentObject(objKey);
    wcollector.setOffset(offset);

    SECTION("addItem mutates the key") {
        wcollector.addItem(ItemKeys::inObject(1), Object3D());
        REQUIRE(collector._lastAdded == ItemKeys::inWorld(chunkKey, objKey, 1));
    }

    SECTION("addMaterial mutates the key") {
        wcollector.addMaterial(ItemKeys::inObject(1), Material("name"));
        REQUIRE(collector._lastMaterialAdded == ItemKeys::inWorld(chunkKey, objKey, 1));
    }

    vec3d position{5, 6, 7};

    SECTION("addItem mutates object position") {
        Object3D obj;
        obj.setPosition(position);
        wcollector.addItem(ItemKeys::inObject(0), obj);
        REQUIRE((offset + position - collector._lastAddedPosition).norm() == Approx(0));
    }
}
