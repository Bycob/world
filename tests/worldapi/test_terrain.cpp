#include <catch/catch.hpp>

#include <worldapi/terrain/Terrain.h>

using namespace world;

TEST_CASE("Terrain - getZInterpolated", "[terrain]") {
	Terrain terrain(2);
	terrain(0, 0) = 1;
	terrain(0, 1) = 0;
	terrain(1, 0) = 0;
	terrain(1, 1) = 1;

	SECTION("getZInterpolated trivial coordinates") {
		REQUIRE(terrain.getZInterpolated(0, 0) == Approx(1));
		REQUIRE(terrain.getZInterpolated(0, 1) == Approx(0));
		REQUIRE(terrain.getZInterpolated(1, 0) == Approx(0));
		REQUIRE(terrain.getZInterpolated(1, 1) == Approx(1));
	}

	SECTION("getZIntepolated non trivial coordinates") {
		REQUIRE(terrain.getZInterpolated(0.5, 0) == Approx(0.5));
		REQUIRE(terrain.getZInterpolated(0, 0.5) == Approx(0.5));
		REQUIRE(terrain.getZInterpolated(0.5, 1) == Approx(0.5));
		REQUIRE(terrain.getZInterpolated(1, 0.5) == Approx(0.5));
	}

	terrain(0, 0) = 0.1;
	terrain(0, 1) = 0.4;
	terrain(1, 0) = 0.5;
	terrain(1, 1) = 0.7;

	SECTION("getZInterpolated trivial coordinates, non trivial heights") {
		REQUIRE(terrain.getZInterpolated(0, 0) == Approx(0.1));
		REQUIRE(terrain.getZInterpolated(0, 1) == Approx(0.4));
		REQUIRE(terrain.getZInterpolated(1, 0) == Approx(0.5));
		REQUIRE(terrain.getZInterpolated(1, 1) == Approx(0.7));
	}

	SECTION("getZIntepolated non trivial coordinates, non trivial heights") {

#ifdef TERRAIN_DEBUG_INFO
		for (double y = 1; y >= -0.02; y -= 0.05) {
			for (double x = 0; x <= 1.01; x += 0.05) {
				std::cout << terrain.getZInterpolated(x, y) << " ";
			}
			std::cout << std::endl;
		}
#endif // TERRAIN_DEBUG_INFO

		REQUIRE(terrain.getZInterpolated(0.5, 0) == Approx(0.3));
		REQUIRE(terrain.getZInterpolated(0, 0.5) == Approx(0.25));
		REQUIRE(terrain.getZInterpolated(0.5, 1) == Approx(0.55));
		REQUIRE(terrain.getZInterpolated(1, 0.5) == Approx(0.6));
	}
}