#include <catch/catch.hpp>

#include <worldapi/maths/Perlin.h>

using namespace world;

TEST_CASE("Perlin - General test case", "[perlin]") {
    Perlin perlin;
    arma::mat reference(100, 100, arma::fill::zeros);
    arma::mat subject(100, 100, arma::fill::zeros);

    SECTION("generating perlin noise changes the matrix") {
        perlin.generatePerlinNoise2D(subject, 0, 1, 4, 0.5);
        REQUIRE(arma::accu(abs(subject-reference)) > 1e-3);
    }

    SECTION("two patterns generated in a row are different") {
        perlin.generatePerlinNoise2D(subject, 0, 1, 4, 0.5);
        perlin.generatePerlinNoise2D(reference, 0, 1, 4, 0.5);
        REQUIRE(arma::accu(abs(subject-reference)) > 1e-3);
    }

    // test sanity check
    SECTION("test sanity check") {
        REQUIRE(arma::accu(abs(subject-reference)) < 1e-3);
    }
}