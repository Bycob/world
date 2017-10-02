#include <iostream>
#include <stdexcept>

#include <worldapi/world/MapGenerator.h>
#include <worldapi/IOUtil.h>

#include "testutil.h"

using namespace img;

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

	ioutil::createDirectory("world");
	ioutil::createDirectory("world/relief");

	std::cout << "Creation du generateur" << std::endl;

	MapGenerator generator(800, 500);
	generator.createReliefMapGenerator<CustomWorldRMGenerator>(biomeDensity, (uint32_t) limitBrightness);
	
	std::cout << "Generation de la ReliefMap" << std::endl;

	auto generated = generator.generate();

	std::cout << "Conversion en image et ecriture" << std::endl;

	Image img = generated->getReliefMapAsImage();
	img.write("world/relief/reliefmap.png");
}
