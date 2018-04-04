#include <iostream>
#include <stdexcept>

#include <worldcore.h>
#include <worldterrain.h>

#include "testutil.h"

using namespace world;

void testReliefMap(int, char**);

int main(int argc, char** argv) {
	testReliefMap(argc, argv);
}

void testReliefMap(int argc, char** argv) {
	int limitBrightness = 4;
	double biomeDensity = 0.02;

	std::cout << "Indiquez la densité des biomes : ";
	parseDouble(std::cin, biomeDensity);

	std::cout << "Indiquez la netteté des limites : ";
	parseInt(std::cin, limitBrightness);
	
	std::cout << "Generation du dossier world/relief" << std::endl;

	world::createDirectory("world");
	world::createDirectory("world/relief");

	std::cout << "Creation du generateur" << std::endl;

	CustomWorldRMModifier generator(biomeDensity, (uint32_t) limitBrightness);
	generator.setMapResolution(513);
	
	std::cout << "Generation de la ReliefMap" << std::endl;

	auto &result = generator.obtainMap(0, 0);
	auto &height = result.first;
	auto &heightDiff = result.second;

	std::cout << "Conversion en image et ecriture" << std::endl;

    height.createImage().write("world/relief/height.png");
    heightDiff.createImage().write("world/relief/heightDiff.png");
}
