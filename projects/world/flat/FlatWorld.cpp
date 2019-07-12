#include "FlatWorld.h"

#include "world/core/LODGridChunkSystem.h"
#include "world/terrain/HeightmapGround.h"
#include "world/tree/ForestLayer.h"
#include "world/tree/SimpleTreeDecorator.h"
#include "world/core/Profiler.h"

namespace world {

class PFlatWorld {
public:
    std::unique_ptr<GroundNode> _ground;

    PFlatWorld() {}
};

FlatWorld *FlatWorld::createDemoFlatWorld() {
    FlatWorld *world = new FlatWorld();

    HeightmapGround &ground = world->setGround<HeightmapGround>();
    ground.setDefaultWorkerSet();

    auto &chunkSystem = world->addPrimaryNode<LODGridChunkSystem>({0, 0, 0});
    chunkSystem.addDecorator<ForestLayer>(world);

    return world;
}


FlatWorld::FlatWorld() : _internal(new PFlatWorld()) {
    auto &ground = setGround<HeightmapGround>();
}

FlatWorld::~FlatWorld() { delete _internal; }

IGround &FlatWorld::ground() { return *_internal->_ground; }

void FlatWorld::collect(ICollector &collector,
                        const IResolutionModel &resolutionModel) {
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

IEnvironment *FlatWorld::getInitialEnvironment() { return this; }

void FlatWorld::setGroundInternal(GroundNode *ground) {
    _internal->_ground = std::unique_ptr<GroundNode>(ground);
}

} // namespace world
