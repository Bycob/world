#include <iostream>
#include <stdexcept>
#include <memory>
#include <chrono>

#include <worldcore.h>
#include <worldflat.h>

using namespace world;

void generate_test_world(int argc, char** argv);

int main(int argc, char** argv) {
    generate_test_world(argc, argv);
}

void generate_test_world(int argc, char** argv) {
	world::createDirectories("assets/world");

    std::cout << "Creation du monde" << std::endl;
    std::unique_ptr<FlatWorld> world(FlatWorld::createDemoFlatWorld());

    std::cout << "Creation de l'explorer et du collecteur" << std::endl;
    FirstPersonExplorer explorer;
    explorer.setPosition({0, 0, 0});
    FlatWorldCollector collector;

    std::cout << "Exploration du monde..." << std::endl;
    Profiler profiler;
    profiler.endStartSection("First exploration");
    explorer.explore<FlatWorld>(*world, collector);
    profiler.endSection();

    std::cout << "Collecte des resultats et ecriture de la scene..." << std::endl;
	Scene scene;
	collector.fillScene(scene);

	ObjLoader loader;
	loader.write(scene, "assets/world/world.obj");

	std::cout << "We explore the same place (which is now generated) for comparison..." << std::endl;
    profiler.endStartSection("Second exploration");
    explorer.explore<FlatWorld>(*world, collector);
    profiler.endSection();

    profiler.dump();
}