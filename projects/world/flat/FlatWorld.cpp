#include "FlatWorld.h"

#include "world/core/LODGridChunkSystem.h"
#include "world/terrain/HeightmapGround.h"
#include "world/tree/ForestLayer.h"
#include "world/tree/SimpleTreeDecorator.h"

namespace world {

class PFlatWorld {
public:
    std::unique_ptr<GroundNode> _ground;

    PFlatWorld() {}
};

FlatWorld *FlatWorld::createDemoFlatWorld() {
    FlatWorld *world = new FlatWorld();

    auto &chunkSystem = world->addPrimaryNode<LODGridChunkSystem>({0, 0, 0});
    chunkSystem.addDecorator<ForestLayer>(world);

    return world;
}


FlatWorld::FlatWorld() : _internal(new PFlatWorld()) {
    auto &ground = setGround<HeightmapGround>();
    ground.setDefaultWorkerSet();
}

FlatWorld::~FlatWorld() { delete _internal; }

IGround &FlatWorld::ground() { return *_internal->_ground; }

void FlatWorld::collect(ICollector &collector,
                        const IResolutionModel &resolutionModel) {
    _internal->_ground->collect(collector, resolutionModel);
    World::collect(collector, resolutionModel);
}

void FlatWorld::setGroundInternal(GroundNode *ground) {
    _internal->_ground = std::unique_ptr<GroundNode>(ground);
}

} // namespace world
