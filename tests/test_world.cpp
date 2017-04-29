#include <iostream>
#include <stdexcept>
#include <memory>

#include <worldapi/world/WorldGenerator.h>

void generate_test_world(int argc, char** argv);

int main(int argc, char** argv) {
    generate_test_world(argc, argv);
}

void generate_test_world(int argc, char** argv) {
    std::cout << "Configuration du g�n�rateur..." << std::endl;
    std::unique_ptr<WorldGenerator> generator(WorldGenerator::defaultGenerator());

    std::cout << "G�neration du monde..." << std::endl;
    std::unique_ptr<World> world(generator->generate());
}