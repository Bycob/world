#include <catch/catch.hpp>

#include <world/core.h>

using namespace world;

TEST_CASE("Perlin - General test case", "[perlin]") {
    Perlin perlin;
    arma::mat reference(100, 100, arma::fill::zeros);
    arma::mat subject(100, 100, arma::fill::zeros);

    SECTION("generating perlin noise changes the matrix") {
        perlin.generatePerlinNoise2D(subject, {1, 0.5, false, 0, 4., 0, 0});
        REQUIRE(arma::accu(arma::abs(subject-reference)) > 1e-3);
    }

    SECTION("two patterns generated in a row are different") {
        perlin.generatePerlinNoise2D(subject, {1, 0.5, false, 0, 4., 0, 0});
        perlin.generatePerlinNoise2D(reference, {1, 0.5, false, 0, 4., 0, 0});
        REQUIRE(arma::accu(arma::abs(subject-reference)) > 1e-3);
    }

    // test sanity check
    SECTION("test sanity check") {
        REQUIRE(arma::accu(arma::abs(subject-reference)) < 1e-3);
    }
}

TEST_CASE("Perlin - Random values modifier") {
	Perlin perlin;
	arma::mat noise(100, 100);

	SECTION("sides must be 0") {
		const double eps = std::numeric_limits<double>::epsilon();
		perlin.generatePerlinNoise2D(noise, {1, 0.5, false, 0, 4., 0, 0}, [&](double x, double y, double val) {
			if (x < eps || 1 - x < eps || y < eps || 1 - y < eps) {
				return .0;
			}

			return val;
		});

		// Sum all the sides : must be ~0
		for (int x = 0; x < 100; x++) {
			for (int y = 0; y < 100; y++) {
				if (x == 0 || y == 0 || x == 99 || y == 99) {
					REQUIRE(noise(x, y) == Approx(0.0));
				}
			}
		}

		// But the noise should not be 0
		REQUIRE(arma::accu(arma::abs(noise)) > 98 * 98 * eps);
	}
}