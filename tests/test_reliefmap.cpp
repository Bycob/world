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
	std::cout << "Generation du dossier world/relief" << std::endl;

	ioutil::createDirectory("world");
	ioutil::createDirectory("world/relief");

	std::cout << "Creation du generateur" << std::endl;

	WorldMapGenerator generator(100, 150);
	generator.emplaceReliefMapGenerator<CustomWorldRMGenerator>();

	std::cout << "Generation de la ReliefMap" << std::endl;

	auto generated = generator.generate();

	std::cout << "Conversion en image et ecriture" << std::endl;

	Image img = generated->getReliefMapAsImage();
	img.write("world/relief/reliefmap.png");
}