#include <catch/catch.hpp>

#include <worldcore.h>

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

    SECTION("integer fast exponential with integer base") {
        REQUIRE(powi(5, 7) == 78125);
        REQUIRE(powi(5, 0) == 1);
        REQUIRE(powi(5, -7) == 0);
    }

    SECTION("integer fast exponential with double base") {
        REQUIRE(powi(5.0, 7) == Approx(78125.0));
        REQUIRE(powi(5.0, 0) == Approx(1.0));
        REQUIRE(powi(5.0, -7) == Approx(1.0 / 78125.0));
    }
}