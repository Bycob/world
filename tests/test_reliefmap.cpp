#include <iostream>
#include <stdexcept>

#include <world/core.h>
#include <world/terrain.h>

#include "testutil.h"

using namespace world;

void testReliefMap(int, char **);
void testLightning(int argc, char **argv);
void testHSV(int argc, char **argv);

int main(int argc, char **argv) {
    // testReliefMap(argc, argv);
    testHSV(argc, argv);
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
    auto &height = result._height;
    auto &heightDiff = result._diff;

    std::cout << "Conversion en image et ecriture" << std::endl;

    height.createImage().write("assets/reliefmap/height.png");
    heightDiff.createImage().write("assets/reliefmap/heightDiff.png");
}

void testLightning(int argc, char **argv) {
    Image img(1024, 1024, ImageType::RGB);
    ImageUtils::fill(img, Color4d(0, 0, 0));
    /*
    ImageUtils::drawLine(img, {54, -56}, {873, 452}, 3, Color4d(0.9, 0.95,
    0.97)); ImageUtils::drawLine(img, {-54, 856}, {473, 52}, 3, Color4d(0.9,
    0.95, 0.97));
     */

    JitterLightning lightning;
    lightning.generateLightning(img, {500, 200}, {500, 800});

    img.write("assets/lightning.png");
}

void testHSV(int argc, char **argv) {
    const int size = 1024;
    Image img(size, size, ImageType::RGB);

    for (int x = 0; x < size; ++x) {
        for (int y = 0; y < size; ++y) {
            img.rgb(x, y) =
                ColorOps::fromHSV(double(x) / size, 1 - double(y) / size,
                                  1 - double(x + y) / (size * 2));
        }
    }

    img.write("assets/hsv_test.png");
}
