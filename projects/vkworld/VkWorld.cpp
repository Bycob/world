#include "VkWorld.h"

#include "world/core/LODGridChunkSystem.h"
#include "world/core/InstancePool.h"
#include "world/core/Profiler.h"
#include "world/core/SeedDistribution.h"
#include "world/flat/FlatWorld.h"
#include "world/tree/ForestLayer.h"
#include "world/nature/Rocks.h"
#include "world/terrain/HeightmapGround.h"
#include "world/terrain/PerlinTerrainGenerator.h"
#include "world/terrain/ReliefMapModifier.h"
#include "world/terrain/MultilayerGroundTexture.h"

#include "VkwMultilayerGroundTexture.h"
#include "VkwGrass.h"
#include "VkwGroundTextureGenerator.h"

namespace world {

void testCPUMultilayer(HeightmapGround &ground) {
    auto &tex = ground.addWorker<MultilayerGroundTexture>();
    tex.addLayer(DistributionParams{-1, 0, 1, 2, // h
                                    -1, 0, 1, 2, // dh
                                    0, 1, 0, 1, 0.2});
    tex.addLayer(DistributionParams{0.33, 0.4, 0.6, 0.75, // h
                                    -1, 0, 0.4, 0.9,      // dh
                                    0, 0.85, 0.25, 0.85, 0.2});
    tex.addLayer(DistributionParams{0.33, 0.4, 0.6, 0.7, // h
                                    -1, 0, 0.2, 0.6,     // dh
                                    0., 1., 0.25, 0.6, 0.2});
    auto &texGen = tex.setTextureProvider<VkwGroundTextureGenerator>();
    texGen.setBaseWorldWidth(100);
    texGen.addLayer("texture-rock.frag");
    texGen.addLayer("texture-soil.frag");
    texGen.addLayer("texture-grass.frag");
}

FlatWorld *VkWorld::createDemoFlatWorld() {
    FlatWorld *world = new FlatWorld();

    HeightmapGround &ground = world->setGround<HeightmapGround>();
    ground.setMaxLOD(8);
    ground.setTextureRes(128);

    ground.addWorker<PerlinTerrainGenerator>(3, 4., 0.35).setMaxOctaveCount(6);

    auto &map = ground.addWorker<CustomWorldRMModifier>();
    map.setRegion({0, 0}, 10000, 3, 0.1, 0.3);
    map.setRegion({0, 0}, 6000, 0.7, 1.6, 0.8);

    ground.addWorker<VkwMultilayerGroundTexture>().addDefaultLayers();

    auto &chunkSystem = world->addPrimaryNode<LODGridChunkSystem>({0, 0, 0});
    chunkSystem.addDecorator<ForestLayer>(world);

    // Grass with seed distribution
    auto &grassPool =
        chunkSystem.addDecorator<InstancePool<VkwGrass, SeedDistribution>>(
            world);

    // Rocks
    auto &rocksPool = chunkSystem.addDecorator<InstancePool<Rocks>>(world);
    rocksPool.distribution().setDensity(0.02);

    auto &rocks = rocksPool.addGenerator();
    rocks.setRadius(0.7);

    for (int i = 0; i < 10; ++i) {
        rocks.addRock({0, 0, 0});
    }

    return world;
}
} // namespace world