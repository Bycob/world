#include <iostream>
#include <stdexcept>

#include <worldapi/world/WorldMapGenerator.h>
#include <worldapi/ioutil.h>

using namespace img;

void testReliefMap(int, char**);

int main(int argc, char** argv) {
	testReliefMap(argc, argv);
}

void testReliefMap(int argc, char** argv) {
	uint32_t limitBrightness = 4;
	double biomeDensity = 0.02;

	std::cout << "Indiquez la densité des biomes : ";
	std::cin >> biomeDensity;

	std::cout << "Indiquez la netteté des limites : ";
	std::cin >> limitBrightness;
	
	std::cout << "Generation du dossier world/relief" << std::endl;

	ioutil::createDirectory("world");
	ioutil::createDirectory("world/relief");

	std::cout << "Creation du generateur" << std::endl;

	WorldMapGenerator generator(500, 800);
	generator.emplaceReliefMapGenerator<CustomWorldRMGenerator>(biomeDensity, limitBrightness);
	
	std::cout << "Generation de la ReliefMap" << std::endl;

	auto generated = generator.generate();

	std::cout << "Conversion en image et ecriture" << std::endl;

	Image img = generated->getReliefMapAsImage();
	img.write("world/relief/reliefmap.png");
}