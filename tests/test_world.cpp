#include <iostream>
#include <stdexcept>
#include <memory>

#include <worldapi/world/WorldGenerator.h>

void generate_test_world(int argc, char** argv);

int main(int argc, char** argv) {
    generate_test_world(argc, argv);
}

void generate_test_world(int argc, char** argv) {
    std::cout << "Géneration du monde..." << std::endl;
    std::unique_ptr<World> world(World::createDemoWorld());
}