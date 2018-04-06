#include <catch/catch.hpp>

#include <worldcore.h>

using namespace world;

TEST_CASE("Types - Integer and float types", "[types]") {

    SECTION("signed int") {
        REQUIRE(sizeof(s8) == 1);
        REQUIRE(sizeof(s16) == 2);
        REQUIRE(sizeof(s32) == 4);
        REQUIRE(sizeof(s64) == 8);
    }

    SECTION("unsigned int") {
        REQUIRE(sizeof(u8) == 1);
        REQUIRE(sizeof(u16) == 2);
        REQUIRE(sizeof(u32) == 4);
        REQUIRE(sizeof(u64) == 8);
    }

    SECTION("floating point values") {
        REQUIRE(sizeof(f32) == 4);
        REQUIRE(sizeof(f64) == 8);
    }
}
