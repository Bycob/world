#include <iostream>
#include <stdexcept>
#include <memory>

#include <worldcore.h>
#include <worldflat.h>

using namespace world;

void generate_test_world(int argc, char** argv);

int main(int argc, char** argv) {
    generate_test_world(argc, argv);
}

void generate_test_world(int argc, char** argv) {
    std::cout << "Creation du monde" << std::endl;
    std::unique_ptr<FlatWorld> world(FlatWorld::createDemoFlatWorld());

    std::cout << "Creation de l'explorer et du collecteur" << std::endl;
    FirstPersonExplorer explorer(1000, 70, 0.5);
    explorer.setPosition({0, 0, 0});
    FlatWorldCollector collector;

    std::cout << "Exploration du monde..." << std::endl;
    explorer.explore<FlatWorld>(*world, collector);

    std::cout << "Exploration termin�e. Collecte des r�sultats" << std::endl;
    auto it = collector.iterateItems();

    while (it.hasNext()) {
        ++it;
    }
}