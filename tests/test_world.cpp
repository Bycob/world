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
	world::createDirectory("world");
	world::createDirectory("world/world");

    std::cout << "Creation du monde" << std::endl;
    std::unique_ptr<FlatWorld> world(FlatWorld::createDemoFlatWorld());

    std::cout << "Creation de l'explorer et du collecteur" << std::endl;
    FirstPersonExplorer explorer;
    explorer.setPosition({0, 0, 0});
    FlatWorldCollector collector;

    std::cout << "Exploration du monde..." << std::endl;
    explorer.explore<FlatWorld>(*world, collector);

    std::cout << "Exploration terminee. Collecte des resultats et ecriture de la scene" << std::endl;
	std::unique_ptr<Scene> scene(collector.createScene());

	ObjLoader loader;
	loader.write(*scene, "world/world/world.obj");
}