#include <catch/catch.hpp>

#include <world/core.h>

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

TEST_CASE("Bounding box", "[math]") {
    BoundingBox bbox;

    SECTION("add point") {
        // Points generated with random.org
        vec3d points[] = {
            {0.0931959087, 0.3047905515, 0.6859918448},
            {0.5158539728, 0.9085651401, 0.5314731595},
            {0.2018859499, 0.4088210356, 0.2293943995},
            {0.7919055360, 0.7573549862, 0.4555407495},
            {0.0044395848, 0.2859698819, 0.6574858558},
            {0.6921222978, 0.6854456297, 0.7073222510},
            {0.7402779224, 0.5797484260, 0.2564249857},
            {0.5544457812, 0.4867092335, 0.0642276097},
            {0.1179276228, 0.0407146120, 0.0963513944},
            {0.7508715644, 0.8558748203, 0.8228310264},
        };

        int i = 0;
        for (auto pt : points) {
            if (i == 0) {
                bbox.reset(pt);
            } else {
                bbox.addPoint(pt);
            }
            ++i;
        }
        INFO(bbox.getLowerBound());
        INFO(bbox.getUpperBound());
        CHECK((bbox.getLowerBound() -
               vec3d{0.0044395848, 0.0407146120, 0.0642276097})
                  .norm() == Approx(0));
        CHECK((bbox.getUpperBound() -
               vec3d{0.7919055360, 0.9085651401, 0.8228310264})
                  .norm() == Approx(0));
    }
}