#include "World.h"

#include <map>

#include "WorldGenerator.h"
#include "Chunk.h"
#include "IWorldExpander.h"

class PrivateWorld {
public:
    PrivateWorld() {}

	std::map<ChunkPosition, std::unique_ptr<Chunk>> _chunks;
	std::vector<std::unique_ptr<IWorldExpander>> _expanders;
};


World::World() : _internal(new PrivateWorld()), _directory() {

}

World::~World() {
    delete _internal;
}

void World::addExpander(IWorldExpander * expander) {
	_internal->_expanders.emplace_back(expander);
}

Chunk & World::getChunk(const ChunkPosition & position) {
	return *_internal->_chunks[position];
}

void World::expand(const IPointOfView & from) {
	for (auto & expander : _internal->_expanders) {
		expander->expand(*this, from);
	}
}
