#include <catch/catch.hpp>

#include <worldcore.h>

using namespace world;

TEST_CASE("ItemKeys", "[collector]") {

    SECTION("Part keys comparison") {
        ItemKey key1 = ItemKeys::inObject(0);
        ItemKey key2 = ItemKeys::inObject(0);
        REQUIRE(key1 == key2);
        REQUIRE_FALSE(key1 < key2);
        REQUIRE_FALSE(key2 < key1);
    }

    SECTION("toString / fromString") {
        ItemKey key1 = ItemKeys::inWorld("0001", 18, 27);
        ItemKey key2 = key(ItemKeys::toString(key1));
        CHECK(key1 == key2);
        CHECK(ItemKeys::toString(key1) == ItemKeys::toString(key2));

        std::string str1 = "fffffffefffffffffffffffffffffffffffffffeffffffffffffffffffffffff"
                          "0100000000000000000000000100000000000000010000000100000000000000"
                          "0000000002000000/0/3202";
        CHECK(str1 == ItemKeys::toString(key(str1)));
    }
}


template <typename T> 
class TestCollectorChannel : public ICollectorChannel<T> {
public:
    
	void put(const ItemKey &key, const T &item) override {
		_lastAdded = key;
	}

	bool has(const ItemKey &key) const override {
		return _lastAdded == key;
	}

	void remove(const ItemKey &key) override {
		_lastRemoved = key;
	}

    ItemKey _lastAdded;
    vec3d _lastAddedPosition;
    std::string _lastAddedMatId;
    ItemKey _lastRemoved;
};

template <>
inline void TestCollectorChannel<Object3D>::put(const ItemKey &key, const Object3D &item) {
    _lastAdded = key;
    _lastAddedPosition = item.getPosition();
    _lastAddedMatId = item.getMaterialID();
}

TEST_CASE("Collector", "[collector]") {
    Collector collector;
    collector.addStorageChannel<Object3D>();

    REQUIRE(collector.hasChannel<Object3D>());
    REQUIRE(collector.hasStorageChannel<Object3D>());
}

TEST_CASE("CollectorContextWrap", "[collector]") {
    Collector collector;
	auto &objChan = collector.addCustomChannel<Object3D, TestCollectorChannel<Object3D>>();
    CollectorContextWrap wcollector(collector);
	auto &wobjChan = wcollector.getChannel<Object3D>();

    ChunkKey chunkKey("001122");
    ObjectKey objKey(2);
    vec3d offset{1, 1, 1};

    SECTION("default context wrap just pass arguments") {
        Object3D obj;
        obj.setPosition(offset);
        wobjChan.put(ItemKeys::inWorld(chunkKey, objKey, 1), obj);
        REQUIRE(objChan._lastAdded == ItemKeys::inWorld(chunkKey, objKey, 1));
        REQUIRE((objChan._lastAddedPosition - offset).norm() == Approx(0));
    }

    wcollector.setCurrentChunk(chunkKey);
    wcollector.setCurrentObject(objKey);
    wcollector.setOffset(offset);

    SECTION("put mutates the key") {
        wobjChan.put(ItemKeys::inObject(1), Object3D());
        REQUIRE(objChan._lastAdded == ItemKeys::inWorld(chunkKey, objKey, 1));
    }

    vec3d position{5, 6, 7};

    SECTION("put mutates object position") {
        Object3D obj;
        obj.setPosition(position);
        wobjChan.put(ItemKeys::inObject(0), obj);
        CAPTURE(objChan._lastAddedPosition);
        REQUIRE((offset + position - objChan._lastAddedPosition).norm() == Approx(0));
    }

    wcollector.setKeyOffset(1);

    SECTION("key is mutated according to the keyoffset") {
        wcollector.setKeyOffset(1);
        Object3D obj;
        wobjChan.put(ItemKeys::inObject(5), obj);
        CHECK(objChan._lastAdded == ItemKeys::inWorld(chunkKey, objKey, 6));
    }

    SECTION("put mutates material id") {
        Object3D obj;
        obj.setMaterialID(ItemKeys::toString(ItemKeys::inObject(1)));
        wobjChan.put(ItemKeys::inObject(1), obj);
        ItemKey afterMutation = ItemKeys::inWorld(chunkKey, objKey, 2);
        CHECK(objChan._lastAddedMatId == ItemKeys::toString(afterMutation));
    }
}
