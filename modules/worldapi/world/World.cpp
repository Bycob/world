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

	std::map<int, LODData> _lodData;
	std::map<ChunkID, std::unique_ptr<Chunk>> _chunks;

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

void World::setLODData(int lod, const LODData & data) {
	_internal->_lodData[lod] = data;
}

LODData & World::getLODData(int lod) const {
	return _internal->_lodData.at(lod);
}

void World::addObject(WorldObject * object) {
	Chunk & chunk = getOrCreateChunk(getChunkPosition(object->getPosition()));
	chunk.addObject(object);
}

void World::addChunkDecorator(IWorldChunkDecorator * decorator) {
	_internal->_chunkDecorators.emplace_back(decorator);
}

bool World::isChunkGenerated(const ChunkID & position) const {
	auto & chunks = _internal->_chunks;
	return chunks.find(position) != chunks.end();
}

Chunk & World::getChunk(const ChunkID & position) {
	if (!isChunkGenerated(position)) {
        Chunk& chunk = getOrCreateChunk(position);
        generateChunk(chunk);
        return chunk;
    }
    else {
        return getOrCreateChunk(position);
    }
}

ChunkID World::getChunkPosition(const ObjectPosition & position) {
	const vec3d & pos3D = position.getPosition3D();
    // TODO à supprimer
	
	return ChunkID(0, 0, 0, 0);
}

void World::generateChunk(Chunk & chunk) {
	for (auto & decorator : _internal->_chunkDecorators) {
		decorator->decorate(*this, chunk);
	}
}

LODData & World::getOrCreateLODData(int lod) {
	auto lodData = _internal->_lodData.find(lod);

	if (lodData == _internal->_lodData.end()) {
        auto dimensions = _internal->_lodData[0].getChunkSize() * pow(2, -lod);
		return _internal->_lodData[lod] = LODData(dimensions);
	}
	else {
		return (*lodData).second;
	}
}

Chunk & World::getOrCreateChunk(const ChunkID & position) {
	auto chunk = _internal->_chunks.find(position);

	if (chunk == _internal->_chunks.end()) {
		// Creation d'un nouveau chunk
		LODData & lodData = getOrCreateLODData(position.getLOD());
		return *(_internal->_chunks[position] = std::make_unique<Chunk>(position, lodData.getChunkSize()));
	}
	else {
		// Retour du chunk existant
		return *(*chunk).second;
	}
}
