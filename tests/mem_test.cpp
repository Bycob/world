#include <iostream>
#include <stdexcept>
#include <memory>
#include <sys/resource.h>

#include <world/core.h>
#include <world/flat.h>


using namespace world;

void generate_test_world(int argc, char **argv);

int main(int argc, char **argv) { generate_test_world(argc, argv); }

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

FlatWorld *createWorld() { return FlatWorld::createDemoFlatWorld(); }

#endif

void generate_test_world(int argc, char **argv) {
    int itcount = 5;

    if (argc > 1) {
        try {
            itcount = std::stoi(argv[1]);
        } catch (...) {
            std::cout << "Unreadable argument" << std::endl;
        }
    }

    std::cout << "Create world" << std::endl;
    std::unique_ptr<FlatWorld> world(createWorld());

    std::cout << "Create explorer and collector" << std::endl;
    FirstPersonView fpsView(700);
    fpsView.setFarDistance(10000);

    Collector collector(CollectorPresets::SCENE);

    double step = 20000;

    for (int i = 0; i < itcount; ++i) {
        double z = world->ground().observeAltitudeAt(i * step, 0, 1);
        vec3d pos{i * step, 0, z + 5};
        fpsView.setPosition(pos);
        std::cout << "Explorer position is " << pos << std::endl;

        collector.reset();
        std::cout << "Explore the world..." << std::endl;
        world->collect(collector, fpsView);
        std::cout << "Done!" << std::endl;

        // Print memory information
        rusage usage;
        getrusage(RUSAGE_SELF, &usage);
        std::cout << "Max memory used (kB): " << usage.ru_maxrss << std::endl;
    }
}