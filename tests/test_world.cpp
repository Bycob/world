#include <iostream>
#include <stdexcept>
#include <memory>
#include <chrono>

#include <world/core.h>
#include <world/flat.h>
#include <world/terrain.h>
#include <world/tree.h>


using namespace world;

void generate_test_world(int argc, char **argv);

void save_world_config(int argc, char **argv);

int main(int argc, char **argv) { save_world_config(argc, argv); }

#ifdef USE_VKWORLD

#include <vkworld/VkwMultilayerGroundTexture.h>
#include <vkworld/VkWorld.h>

FlatWorld *createWorld() {
    FlatWorld *world = new FlatWorld();
    HeightmapGround &ground = world->setGround<HeightmapGround>();

    ground.setTerrainResolution(65);
    ground.setTextureRes(128);
    ground.setMaxLOD(5);

    ground.addWorker<PerlinTerrainGenerator>(3, 4., 0.35);
    // ground.addWorker<CustomWorldRMModifier>(1);
    ground.addWorker<VkwMultilayerGroundTexture>().addDefaultLayers();

    return world;
}
#else

FlatWorld *createWorld() {
    FlatWorld *world = new FlatWorld();
    HeightmapGround &ground = world->setGround<HeightmapGround>(4000);

    ground.setTerrainResolution(65);
    ground.setTextureRes(16);
    ground.setMaxLOD(8);

    ground.addWorker<PerlinTerrainGenerator>(3, 4., 0.4);
    ground.addWorker<CustomWorldRMModifier>(1);

    auto &chunkSystem = world->addPrimaryNode<GridChunkSystem>({0, 0, 0});
    chunkSystem.addDecorator<ForestLayer>(world);

    return world;
}

#endif

void generate_test_world(int argc, char **argv) {
    world::createDirectories("assets/world");

    std::cout << "Creation du monde" << std::endl;
    std::unique_ptr<FlatWorld> world(createWorld());

    std::cout << "Creation de l'explorer et du collecteur" << std::endl;
    FirstPersonView fpsView;
    double z = world->ground().observeAltitudeAt(0, 0, 1);
    fpsView.setFarDistance(200);
    fpsView.setPosition({0, 0, z + 5});
    std::cout << "Explorer position is " << vec3d{0, 0, z + 5} << std::endl;

    Collector collector(CollectorPresets::SCENE);

    std::cout << "Exploration du monde..." << std::endl;
    Profiler profiler;
    profiler.endStartSection("First exploration");
    world->collect(collector, fpsView);
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
    world->collect(collector, fpsView);
    profiler.endSection();

    profiler.dump();
}

void save_world_config(int argc, char **argv) {
    world::createDirectories("assets");

    World::createDemoWorld()->save("assets/demo.json");
    std::cout << "Saved demo world to assets/demo.json" << std::endl;
}