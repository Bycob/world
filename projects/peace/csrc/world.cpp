#include "common.h"

#include <world/core.h>
#include <world/flat.h>
#include <world/terrain.h>

#if USE_VKWORLD
#include <vkworld/VkWorld.h>
#endif

using namespace world;

extern "C" {

PEACE_EXPORT WorldPtr createTestWorld() {
    auto *world = new FlatWorld();

    auto &ground = world->setGround<HeightmapGround>();
    ground.addWorker<PerlinTerrainGenerator>(3, 4);
    ground.setMaxLOD(3);

    auto &texturer = ground.addWorker<SimpleTexturer>();
    texturer.getColorMap().addPoint({1, 0.5}, Color4d{0, 1, 0});
    texturer.getColorMap().addPoint({0, 0.5}, Color4d{0.3, 0.3, 0});

    return world;
}

PEACE_EXPORT WorldPtr createDemoWorld(char* name) {
    if (name == std::string("vulkan")) {
#ifdef USE_VKWORLD
        return VkWorld::createDemoFlatWorld();
#else
        std::cout << "Not supported ! Return default world instead..." << std::endl;
#endif
    }
    return World::createDemoWorld();
}

}