#include "World.h"

#include <map>

#include "CollectorContextWrap.h"
#include "flat/FlatWorld.h"
#include "LODGridChunkSystem.h"

namespace world {

class PWorld {
public:
    PWorld() = default;

    std::vector<std::unique_ptr<WorldDecorator>> _chunkDecorators;
    /// Keep track of which zone has been generated
    std::set<WorldZone> _generated;
    // TODO Make a more elaborated object
    // -> when a full chunk has been explored at a certain level
    // sublevels are not copied.
};


World *World::createDemoWorld() { return FlatWorld::createDemoFlatWorld(); }


World::World()
        : _internal(new PWorld()),
          _chunkSystem(std::make_unique<LODGridChunkSystem>()), _directory() {}

World::~World() { delete _internal; }

WorldZone World::exploreLocation(const vec3d &location) {
    auto result = _chunkSystem->getChunk(location);
    return result;
}

std::vector<WorldZone> World::exploreNeighbours(const WorldZone &zone) {
    auto result = _chunkSystem->getNeighbourChunks(zone);
    return result;
}

std::vector<WorldZone> World::exploreInside(const WorldZone &zone) {
    auto zones = _chunkSystem->getChildren(zone);
    return zones;
}

void World::collect(const WorldZone &zone, ICollector &collector) {
    // Decorate zone if needed
    // TODO if (shouldDecorate(zone)) {
    if (_internal->_generated.find(zone) == _internal->_generated.end()) {
        onFirstExploration(zone);
        _internal->_generated.insert(zone);
    }

    // Collect zone
    CollectorContextWrap wcollector(collector);
    wcollector.setCurrentChunk(zone->getID());
    wcollector.setOffset(zone->getAbsoluteOffset());

    zone->chunk().collectWholeChunk(wcollector);

    // TODO collect chunks from higher level
}

void World::onFirstExploration(const WorldZone &chunk) {
    for (auto &decorator : _internal->_chunkDecorators) {
        decorator->decorate(*this, chunk);
    }
}

void World::addDecoratorInternal(world::WorldDecorator *decorator) {
    _internal->_chunkDecorators.emplace_back(decorator);
}

Chunk &World::getChunk(const world::WorldZone &zone) {
    return _chunkSystem->getChunk(zone);
}
} // namespace world
