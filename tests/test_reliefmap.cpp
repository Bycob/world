#include <iostream>
#include <stdexcept>

#include <world/core.h>
#include <world/terrain.h>

#include "testutil.h"

using namespace world;

void testReliefMap(int, char **);
void testLightning(int argc, char **argv);

int main(int argc, char **argv) {
    // testReliefMap(argc, argv);
    testLightning(argc, argv);
}

void testReliefMap(int argc, char **argv) {
    int limitBrightness = 4;
    double biomeDensity = 0.02;

    std::cout << "Indiquez la densité des biomes : ";
    parseDouble(std::cin, biomeDensity);

    std::cout << "Indiquez la netteté des limites : ";
    parseInt(std::cin, limitBrightness);

    std::cout << "Generation du dossier testing/reliefmap" << std::endl;

    world::createDirectories("assets/reliefmap/");

    std::cout << "Creation du generateur" << std::endl;

    CustomWorldRMModifier generator(biomeDensity, (uint32_t)limitBrightness);
    generator.setMapResolution(513);

    std::cout << "Generation de la ReliefMap" << std::endl;

    auto &result = generator.obtainMap(0, 0);
    auto &height = result.first;
    auto &heightDiff = result.second;

    std::cout << "Conversion en image et ecriture" << std::endl;

    height.createImage().write("assets/reliefmap/height.png");
    heightDiff.createImage().write("assets/reliefmap/heightDiff.png");
}

void testLightning(int argc, char **argv) {
    Image img(1024, 1024, ImageType::RGB);
    ImageUtils::fill(img, Color4d(0, 0, 0));
    // ImageUtils::drawLine(img, {54, -56}, {873, 452}, 3, Color4d(0.9, 0.95,
    // 0.97));

    Lightning lightning;
    lightning.generateLightning(img, {500, 200});

    img.write("assets/lightning.png");
}
