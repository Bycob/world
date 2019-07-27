#include "common.h"

#include <world/core.h>
#include <world/flat.h>
#include <world/terrain.h>

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

PEACE_EXPORT WorldPtr createDemoWorld() { return World::createDemoWorld(); }
}