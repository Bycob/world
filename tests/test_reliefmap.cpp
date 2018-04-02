#include <iostream>
#include <stdexcept>

#include <worldapi/world/ReliefMap.h>
#include <worldapi/IOUtil.h>

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

	CustomWorldRMGenerator generator(biomeDensity, (uint32_t) limitBrightness);
	
	std::cout << "Generation de la ReliefMap" << std::endl;
	Terrain height(513);
	Terrain heightDiff(513);

	generator.generate(height, heightDiff);

	std::cout << "Conversion en image et ecriture" << std::endl;

    height.createImage().write("world/relief/height.png");
    heightDiff.createImage().write("world/relief/heightDiff.png");
}
