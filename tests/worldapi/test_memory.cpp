#include <catch/catch.hpp>

#include <worldcore.h>

using namespace world;

class MemoryTestClass {
public:
    int _copy;

    MemoryTestClass()
            : _copy(0) {}

    MemoryTestClass(const MemoryTestClass &other)
            : _copy(other._copy + 1) {}
};


TEST_CASE("ref or value", "[memory]") {
    MemoryTestClass test;

    RefOrValue<MemoryTestClass> value(test, false);
    RefOrValue<MemoryTestClass> ref(test, true);

    SECTION("Basics") {
        REQUIRE(ref->_copy == 0);
        REQUIRE(value->_copy == 1);
    }

    SECTION("copy") {
        auto v = ref;
        REQUIRE(v->_copy == 0);
        const auto &r = ref;
        v = r;
        REQUIRE(v->_copy == 1);
    }
}

TEST_CASE("const ref or value", "[memory]") {
    MemoryTestClass test;

    ConstRefOrValue<MemoryTestClass> value(test, false);
    ConstRefOrValue<MemoryTestClass> ref(test, true);

    SECTION("Basics") {
        REQUIRE(ref->_copy == 0);
        REQUIRE(value->_copy == 1);
    }

    SECTION("Move") {
        auto v = std::move(value);
        REQUIRE(v->_copy == 1);
        v = std::move(ref);
        REQUIRE(v->_copy == 0);
    }

    SECTION("Assign - move") {
        auto v = ConstRefOrValue<MemoryTestClass>(test, false);
        REQUIRE(v->_copy == 1);
        v = value;
        REQUIRE(v->_copy == 2);
    }
}