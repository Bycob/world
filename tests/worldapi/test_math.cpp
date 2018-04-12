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

TEST_CASE("vec3i and vec3d - mixed operators", "[math]") {
    vec3i veci{5, -5, 5};
    vec3d vecd{2.1, 2.1, 2.1};

    SECTION("vec3i plus vec3d") {
        auto result = veci + vecd;
        REQUIRE(result.x == Approx(7.1));
        REQUIRE(result.y == Approx(-2.9));
        REQUIRE(result.z == Approx(7.1));
    }

    SECTION("vec3i minus vec3d") {
        auto result = veci - vecd;
        REQUIRE(result.x == Approx(2.9));
        REQUIRE(result.y == Approx(-7.1));
        REQUIRE(result.z == Approx(2.9));
    }

    SECTION("vec3i multiplied by vec3d") {
        auto result = veci * vecd;
        REQUIRE(result.x == Approx(5 * 2.1));
        REQUIRE(result.y == Approx(-5 * 2.1));
        REQUIRE(result.z == Approx(5 * 2.1));
    }

    SECTION("vec3i divided by double") {
        auto result = veci / 2.;
        REQUIRE(result.x == Approx(2.5));
        REQUIRE(result.y == Approx(-2.5));
        REQUIRE(result.z == Approx(2.5));
    }

    vecd = {2., -2., 2.};

    SECTION("vec3i divided by vec3d") {
        auto result = veci / vecd;
        REQUIRE(result.x == 2.5);
        REQUIRE(result.y == 2.5);
        REQUIRE(result.z == 2.5);
    }
}

TEST_CASE("vec2i and vec2d - mixed operators", "[math]") {
    vec2i veci{5, -5};
    vec2d vecd{2.1, 2.1};

    WARN("vec2i and vec2d - mixed operators : not implemented yet");
    /*SECTION("vec3i plus vec3d") {
        auto result = veci + vecd;
        REQUIRE(result.x == Approx(7.1));
        REQUIRE(result.y == Approx(-2.9));
    }

    SECTION("vec3i minus vec3d") {
        auto result = veci - vecd;
        REQUIRE(result.x == Approx(2.9));
        REQUIRE(result.y == Approx(-7.1));
    }

    SECTION("vec3i multiplied by vec3d") {
        auto result = veci * vecd;
        REQUIRE(result.x == Approx(5 * 2.1));
        REQUIRE(result.y == Approx(-5 * 2.1));
    }

    SECTION("vec3i divided by double") {
        auto result = veci / 2.;
        REQUIRE(result.x == Approx(2.5));
        REQUIRE(result.y == Approx(-2.5));
    }

    vecd = {2., -2.};

    SECTION("vec3i divided by vec3d") {
        auto result = veci / vecd;
        REQUIRE(result.x == 2.5);
        REQUIRE(result.y == 2.5);
    }*/
}