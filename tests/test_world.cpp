#include <iostream>
#include <stdexcept>
#include <memory>

#include <worldapi/world/FlatWorld.h>
#include <worldapi/world/FlatWorldCollector.h>
#include <worldapi/world/FirstPersonExplorer.h>

using namespace world;

void generate_test_world(int argc, char** argv);

int main(int argc, char** argv) {
    generate_test_world(argc, argv);
}

void generate_test_world(int argc, char** argv) {
    std::cout << "Creation du monde" << std::endl;
    std::unique_ptr<World> world(World::createDemoWorld());

    std::cout << "Creation de l'explorer et du collecteur" << std::endl;
    FirstPersonExplorer explorer(0.002, 0.5);
    explorer.setOrigin({0, 0, 0});
    FlatWorldCollector collector;

    std::cout << "Exploration du monde..." << std::endl;
    explorer.explore<FlatWorld>(*((FlatWorld*)world.get()), collector);

    std::cout << "Exploration terminée. Collecte des résultats" << std::endl;
    auto it = collector.iterateItems();

    while (!it.hasNext()) {
        ++it;
    }
}