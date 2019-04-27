#include <catch/catch.hpp>

#include <world/core.h>

using namespace world;

TEST_CASE("Perlin - General test case", "[perlin]") {
    Perlin perlin;
    arma::mat reference(100, 100, arma::fill::zeros);
    arma::mat subject(100, 100, arma::fill::zeros);

    REQUIRE(arma::accu(arma::abs(subject - reference)) < 1e-3);

    SECTION("generating perlin noise changes the matrix") {
        perlin.generatePerlinNoise2D(subject, {1, 0.5, false, 0, 4., 0, 0});
        REQUIRE(arma::accu(arma::abs(subject - reference)) > 1e-3);
    }

    SECTION("two patterns generated in a row are the same") {
        perlin.generatePerlinNoise2D(subject, {1, 0.5, false, 0, 4., 0, 0});
        perlin.generatePerlinNoise2D(reference, {1, 0.5, false, 0, 4., 0, 0});
        REQUIRE(arma::accu(arma::abs(subject - reference)) < 1e-3);
    }
}

TEST_CASE("Perlin - Random values modifier") {
    Perlin perlin;
    arma::mat noise(100, 100);

    SECTION("sides must be 0") {
        const double eps = std::numeric_limits<double>::epsilon();
        perlin.generatePerlinNoise2D(noise, {1, 0.5, false, 0, 4., 0, 0},
            [&](double x, double y, double val) {
            if (x < eps || 1 - x < eps ||
                y < eps || 1 - y < eps) {
                return .0;
            }

            return val;
        });

        // Sum all the sides : must be ~0
        bool success = true;
        std::stringstream errors;

        for (int x = 0; x < 100; x++) {
            for (int y = 0; y < 100; y++) {
                if (x == 0 || y == 0 || x == 99 || y == 99) {
                    if (noise(x, y) != Approx(0.0)) {
                        success = false;
                        errors << noise(x, y) << " should be 0" << std::endl;
                    }
                }
            }
        }

        // But the noise should not be 0
        INFO(errors.str());
        CHECK(success);
        CHECK(arma::accu(arma::abs(noise)) > 98 * 98 * eps);
    }
}

TEST_CASE("Perlin - Benchmarks", "[!benchmark]") {
	arma::mat noise(1024, 1024);

	Perlin perlin;
	PerlinInfo info{
		12, 0.4, false, 0, 4, 0, 0
	};

	BENCHMARK("1024*1024 perlin with 4 frequency at octave 0, and 13 octaves") {
		perlin.generatePerlinNoise2D(noise, info);
	}

	arma::mat noise2(4096, 4096);
	info.octaves = 3;
	info.frequency *= 4;

	BENCHMARK("4096*4096 perlin with 16 frequency at octave 0, and 4 octaves") {
		perlin.generatePerlinNoise2D(noise2, info);
	}
}