#include <catch/catch.hpp>

#include <worldcore.h>

using namespace world;

TEST_CASE("ItemKeys", "[collector]") {

    SECTION("Part keys") {
        ItemKey key1 = ItemKeys::inObject(0);
        ItemKey key2 = ItemKeys::inObject(0);
        REQUIRE(key1 == key2);
        REQUIRE_FALSE(key1 < key2);
        REQUIRE_FALSE(key2 < key1);
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

	const T &get(const ItemKey &key) const override {
		throw new std::runtime_error("Not supported");
	};

	std::string keyToString(const ItemKey &key) const override {
		return ItemKeys::toString(key);
	}

    ItemKey _lastAdded;
    vec3d _lastAddedPosition;
    ItemKey _lastRemoved;
};

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

    SECTION("addItem mutates the key") {
        wobjChan.put(ItemKeys::inObject(1), Object3D());
        REQUIRE(objChan._lastAdded == ItemKeys::inWorld(chunkKey, objKey, 1));
    }

    vec3d position{5, 6, 7};

    SECTION("addItem mutates object position") {
        Object3D obj;
        obj.setPosition(position);
        wobjChan.put(ItemKeys::inObject(0), obj);
        REQUIRE((offset + position - objChan._lastAddedPosition).norm() == Approx(0));
    }
}
