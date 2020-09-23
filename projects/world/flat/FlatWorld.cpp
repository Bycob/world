#include <world/terrain/PerlinTerrainGenerator.h>
#include <world/terrain.h>
#include "FlatWorld.h"

#include "world/core/GridChunkSystem.h"
#include "world/core/InstancePool.h"
#include "world/terrain/HeightmapGround.h"
#include "world/tree/ForestLayer.h"
#include "world/nature/Grass.h"
#include "world/tree/SimpleTreeDecorator.h"
#include "world/nature/Rocks.h"
#include "world/core/Profiler.h"
#include "world/core/SeedDistribution.h"
#include "world/terrain/MultilayerGroundTexture.h"
#include "world/terrain/CachedTextureProvider.h"
#include "world/core/WorldFile.h"
#include "world/terrain/GroundBiomes.h"

namespace world {

class PFlatWorld {
public:
    std::unique_ptr<GroundNode> _ground;

    PFlatWorld() {}
};

FlatWorld *FlatWorld::createDemoFlatWorld() {
    FlatWorld *world = new FlatWorld();

    HeightmapGround &ground = world->setGround<HeightmapGround>();

    ground.addWorker<PerlinTerrainGenerator>(3, 4., 0.35).setMaxOctaveCount(6);

    auto &map = ground.addWorker<CustomWorldRMModifier>();
    map.setRegion({0, 0}, 10000, 3, 0.1, 0.3);
    map.setRegion({0, 0}, 6000, 0.7, 1.6, 0.8);

    ground.addWorker<GroundBiomes>();
    ground.addWorker<AltitudeTexturer>();

    /*auto &multilayer = ground.addWorker<MultilayerGroundTexture>();
    multilayer.setTextureProvider<CachedTextureProvider>("multilayer/");
    // rock
    multilayer.addLayer(DistributionParams{-1, 0, 1, 2, // h
                                           -1, 0, 1, 2, // dh
                                           0, 1, 0, 1, 0.05});
    // sand
    multilayer.addLayer(DistributionParams{-1, 0, 0.4, 0.45, // h
                                           -1, 0, 0.4, 0.6,  // dh
                                           0, 1, 0, 1, 0.05});
    // soil
    multilayer.addLayer(DistributionParams{0.33, 0.4, 0.6, 0.75, // h
                                           -1, 0, 0.45, 0.65,    // dh
                                           0, 0.85, 0.25, 0.85, 0.05});
    // grass
    multilayer.addLayer(DistributionParams{0.33, 0.4, 0.6, 0.7, // h
                                           -1, 0, 0.4, 0.6,     // dh
                                           0., 1., 0.25, 0.6, 0.05});
    // snow
    multilayer.addLayer(DistributionParams{0.65, 0.8, 1, 2, // h
                                           -1, 0, 0.5, 0.7, // dh
                                           0.0, 1.0, 0, 1., 0.05});*/


    auto &chunkSystem = world->addPrimaryNode<GridChunkSystem>({0, 0, 0});
    chunkSystem.addDecorator<ForestLayer>();

    // auto &treePool = chunkSystem.addDecorator<InstancePool<Tree>>(world);
    // treePool.setResolution(1);
    // treePool.distribution().setDensity(0.0008);

    // Grass with seed distribution
    auto &grassPool = chunkSystem.addDecorator<InstancePool>();
    grassPool.setDistribution<SeedDistribution>();
    grassPool.setTemplateGenerator<Grass>();

    // Rocks
    auto &rocksPool = chunkSystem.addDecorator<InstancePool>();
    rocksPool.setDistribution<RandomDistribution>().setDensity(0.02);
    rocksPool.setTemplateGenerator<Rocks>();

    auto &rocks = rocksPool.addGenerator<Rocks>();
    rocks.setRadius(0.7);

    for (int i = 0; i < 10; ++i) {
        rocks.addRock({0, 0, 0});
    }

    return world;
}


FlatWorld::FlatWorld() : _internal(new PFlatWorld()) {
    auto &ground = setGround<HeightmapGround>();
}

FlatWorld::~FlatWorld() { delete _internal; }

IGround &FlatWorld::ground() { return *_internal->_ground; }

void FlatWorld::collect(ICollector &collector,
                        const IResolutionModel &resolutionModel) {
    // TODO remove this hack when we can deserialize references
    auto *hg = dynamic_cast<HeightmapGround *>(_internal->_ground.get());
    if (hg != nullptr) {
        _atmosphericProvider = hg->getAtmosphericProvider();
    }
    // end of hack

    _internal->_ground->collect(collector, resolutionModel);
    World::collect(collector, resolutionModel);
}

vec3d FlatWorld::findNearestFreePoint(const vec3d &origin,
                                      const vec3d &direction, double resolution,
                                      const ExplorationContext &ctx) const {
    auto pos = origin + ctx.getOffset();
    double z = _internal->_ground->observeAltitudeAt(pos.x, pos.y, resolution);
    // std::cout << z - ctx.getOffset().z << " <> " << origin.z << std::endl;
    return {origin.x, origin.y, z - ctx.getOffset().z};
}

void FlatWorld::write(WorldFile &wf) const {
    wf.addChild("ground", _internal->_ground->serializeSubclass());
    World::write(wf);
}

void FlatWorld::read(const WorldFile &wf) {
    _internal->_ground.reset(readSubclass<GroundNode>(wf.readChild("ground")));
    World::read(wf);
}

IEnvironment *FlatWorld::getInitialEnvironment() { return this; }

void FlatWorld::setGroundInternal(GroundNode *ground) {
    _internal->_ground = std::unique_ptr<GroundNode>(ground);
}

} // namespace world
