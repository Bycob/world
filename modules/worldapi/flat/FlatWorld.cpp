#include "FlatWorld.h"

#include "terrain/Ground.h"
#include "tree/SimpleTreeDecorator.h"
#include "FlatWorldCollector.h"

namespace world {

class PFlatWorld {
public:
    PFlatWorld() {}

    std::vector<std::unique_ptr<FlatWorldDecorator>> _chunkDecorators;
};

FlatWorld *FlatWorld::createDemoFlatWorld() {
    FlatWorld *world = new FlatWorld();

    world->addFlatWorldDecorator<SimpleTreeDecorator>(3);

    return world;
}

FlatWorld::FlatWorld() : _internal(new PFlatWorld()) {

    Ground &ground = setGround<Ground>();
    ground.setDefaultWorkerSet();
}

FlatWorld::~FlatWorld() { delete _internal; }

IGround &FlatWorld::ground() { return *_ground; }

void world::FlatWorld::collect(const WorldZone &zone, ICollector &collector) {
    World::collect(zone, collector);

    ground().collectZone(zone, collector);
}

void FlatWorld::collect(const WorldZone &zone, FlatWorldCollector &collector) {
    World::collect(zone, collector);

    ground().collectZone(zone, *this, collector);
}

void FlatWorld::onFirstExploration(const WorldZone &chunk) {
    World::onFirstExploration(chunk);

    for (auto &decorator : _internal->_chunkDecorators) {
        decorator->decorate(*this, chunk);
    }
}

void FlatWorld::setGroundInternal(IGround *ground) {
    _ground = std::unique_ptr<IGround>(ground);
}

void FlatWorld::addFlatWorldDecoratorInternal(
    world::FlatWorldDecorator *decorator) {
    _internal->_chunkDecorators.emplace_back(decorator);
}
} // namespace world
