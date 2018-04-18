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
};


World *World::createDemoWorld() { return FlatWorld::createDemoFlatWorld(); }


World::World()
        : _internal(new PWorld()),
          _chunkSystem(std::make_unique<LODGridChunkSystem>()), _directory() {}

World::~World() { delete _internal; }

WorldZone World::exploreLocation(const vec3d &location) {
    auto result = _chunkSystem->getChunk(location);
    WorldZone &zone = result._zone;

    if (result._created) {
        onFirstExploration(zone);
    }
    return result._zone;
}

std::vector<WorldZone> World::exploreNeighbours(const WorldZone &zone) {
    auto pairs = _chunkSystem->getNeighbourChunks(zone);

    std::vector<WorldZone> result;

    for (auto &pair : pairs) {
        WorldZone &nzone = pair._zone;

        if (pair._created) {
            onFirstExploration(nzone);
        }
        result.emplace_back(pair._zone);
    }
    return result;
}

std::vector<WorldZone> World::exploreInside(const WorldZone &zone) {
    auto zones = _chunkSystem->getChildren(zone);
    std::vector<WorldZone> result;

    for (auto &child : zones) {
        if (child._created) {
            onFirstExploration(child._zone);
        }

        result.emplace_back(child._zone);
    }

    return result;
}

void World::collect(const WorldZone &zone, ICollector &collector) {
    CollectorContextWrap wcollector(collector);
    wcollector.setCurrentChunk(zone->getID());
    wcollector.setOffset(zone->getAbsoluteOffset());

    zone->chunk().collectWholeChunk(wcollector);

    // TODO collect chunks from higher level
}

void World::onFirstExploration(WorldZone &chunk) {
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
