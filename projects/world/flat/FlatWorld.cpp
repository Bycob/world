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

    auto &texturer = ground.addWorker<AltitudeTexturer>();
    ColorMap &colorMap = texturer.getColorMap();

    colorMap.addPoint({0.15, 0.5}, Color4u(209, 207, 153)); // Sand
    colorMap.addPoint({0.31, 0}, Color4u(209, 207, 153));   // Sand
    colorMap.addPoint({0.31, 1}, Color4u(209, 207, 153));   // Sand
    colorMap.addPoint({0.35, 0}, Color4u(144, 183, 92));    // Light grass
    colorMap.addPoint({0.35, 1}, Color4u(72, 132, 65));     // Dark grass
    colorMap.addPoint({0.5, 0}, Color4u(144, 183, 100));    // Light grass
    colorMap.addPoint({0.5, 1}, Color4u(96, 76, 40));       // Dark dirt
    colorMap.addPoint({0.75, 0}, Color4u(96, 76, 40));      // Dark dirt
    colorMap.addPoint({0.75, 1}, Color4u(160, 160, 160));   // Rock
    colorMap.addPoint({1, 0}, Color4u(244, 252, 250));      // Snow
    colorMap.addPoint({1, 1}, Color4u(160, 160, 160));      // Rock
    colorMap.setOrder(3);

    // ground.addWorker<GroundBiomes>();
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
    ground.setKey("ground"_k);
    ground.configureCache(_cacheRoot);
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
    double z =
        _internal->_ground->observeAltitudeAt(pos.x, pos.y, resolution, ctx);
    // std::cout << z - ctx.getOffset().z << " <> " << origin.z << std::endl;
    return {origin.x, origin.y, z - ctx.getOffset().z};
}

void FlatWorld::write(WorldFile &wf) const {
    wf.addChild("ground", _internal->_ground->serializeSubclass());
    World::write(wf);
}

void FlatWorld::read(const WorldFile &wf) {
    setGroundInternal(readSubclass<GroundNode>(wf.readChild("ground")));
    World::read(wf);
}

IEnvironment *FlatWorld::getInitialEnvironment() { return this; }

void FlatWorld::setGroundInternal(GroundNode *ground) {
    // <!> The default ground is deleted when calling setGround
    // This may cause problems with the cache in the future
    _internal->_ground = std::unique_ptr<GroundNode>(ground);
    _internal->_ground->setKey("ground"_k);
    _internal->_ground->configureCache(_cacheRoot);
}

} // namespace world
