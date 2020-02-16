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
    bool land = true;
    double max = 180000;

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
    double xpos = -step;
    double ypos = 0;

    for (int i = 0; i < itcount; ++i) {
        xpos += step;
        double z = world->ground().observeAltitudeAt(xpos, ypos, 1);

        while (!land || z < 100 || z > 1500) {
            xpos += z > 1500 ? 10 : 1000;

            if (xpos > max) {
                xpos = -max;
                ypos = ypos > max ? -max : ypos + step;
            }

            z = world->ground().observeAltitudeAt(xpos, ypos, 1);
        }
        vec3d pos{xpos, ypos, z + 5};
        fpsView.setPosition(pos);
        std::cout << "Explorer position is " << pos << std::endl;

        collector.reset();
        std::cout << "Explore the world..." << std::endl;
        world->collect(collector, fpsView);
        std::cout << "Done!" << std::endl;

        // Print memory information
        std::cout << "Max memory used: " << getReadableMemoryUsage(5)
                  << std::endl;
    }
}