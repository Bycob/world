#include <catch/catch.hpp>

#include <worldapi/maths/MathsHelper.h>

using namespace world;

TEST_CASE("Math", "[math]") {

    SECTION("integer modulo") {
        REQUIRE(mod(5, 3) == 2);
        REQUIRE(mod(3, 3) == 0);
        REQUIRE(mod(0, 3) == 0);
        REQUIRE(mod(-2, 3) == 1);
        REQUIRE(mod(-3, 3) == 0);
    }

    SECTION("integer clamp") {
        REQUIRE(clamp(5, -1, 3) == 3);
        REQUIRE(clamp(2, -1, 3) == 2);
        REQUIRE(clamp(-4, -1, 3) == -1);
    }

    SECTION("double clamp with integer bounds") {
        REQUIRE(clamp(3.14, -1, 3) == Approx(3));
        REQUIRE(clamp(2.71, -1, 3) == Approx(2.71));
        REQUIRE(clamp(-6.28, -1, 3) == Approx(-1));
    }
}