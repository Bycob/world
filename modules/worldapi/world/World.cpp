#include "World.h"

#include <map>

#include "FlatWorld.h"
#include "SimpleTreeDecorator.h"
#include "Chunk.h"
#include "LODData.h"

using namespace maths;

class PrivateWorld {
public:
    PrivateWorld() = default;

	std::vector<std::unique_ptr<IWorldChunkDecorator>> _chunkDecorators;
};



World * World::createDemoWorld() {
	FlatWorld * world = new FlatWorld();

    world->addFlatWorldDecorator(new SimpleTreeDecorator());

	return world;
}


World::World() : _internal(new PrivateWorld()), _directory() {

}

World::~World() {
    delete _internal;
}

void World::addChunkDecorator(IWorldChunkDecorator * decorator) {
	_internal->_chunkDecorators.emplace_back(decorator);
}

ChunkNode World::exploreLocation(const maths::vec3d &location) {
	auto pair = _chunkSystem.getOrCreateChunkID(location);
	ChunkNode& chunk = pair.first;

    if (pair.second) {
        onFirstExploration(chunk);
    }
	return pair.first;
}

ChunkNode World::exploreNeighbour(const ChunkNode &chunk, const maths::vec3d &direction) {
	auto pair = _chunkSystem.getOrCreateNeighbourID(chunk, direction);
	ChunkNode& nchunk = pair.first;

    if (pair.second) {
        onFirstExploration(nchunk);
    }
	return pair.first;
}

void World::onFirstExploration(ChunkNode &chunk) {
	for (auto & decorator : _internal->_chunkDecorators) {
		decorator->decorate(*this, chunk);
	}
}
