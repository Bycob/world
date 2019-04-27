#include <iostream>
#include <stdexcept>
#include <memory>
#include <chrono>

#include <world/core.h>
#include <world/flat.h>


using namespace world;

void generate_test_world(int argc, char **argv);

int main(int argc, char **argv) { generate_test_world(argc, argv); }

void generate_test_world(int argc, char **argv) {
    world::createDirectories("assets/world");

    std::cout << "Creation du monde" << std::endl;
    std::unique_ptr<FlatWorld> world(FlatWorld::createDemoFlatWorld());

    std::cout << "Creation de l'explorer et du collecteur" << std::endl;
    FirstPersonView explorer;
    explorer.setPosition({0, 0, 0});

    Collector collector;
    collector.addStorageChannel<Object3D>();
    collector.addStorageChannel<world::Material>();
    collector.addStorageChannel<Image>();

    std::cout << "Exploration du monde..." << std::endl;
    Profiler profiler;
    profiler.endStartSection("First exploration");
    world->collect(collector, explorer);
    profiler.endSection();

    std::cout << "Collecte des resultats et ecriture de la scene..."
              << std::endl;
    Scene scene;
    collector.fillScene(scene);

    ObjLoader loader;
    loader.write(scene, "assets/world/world.obj");

    std::cout << "We explore the same place (which is now generated) for "
                 "comparison..."
              << std::endl;
    profiler.endStartSection("Second exploration");
    world->collect(collector, explorer);
    profiler.endSection();

    profiler.dump();
}