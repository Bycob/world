#include "World.h"

#include <map>

#include "WorldGenerator.h"
#include "Chunk.h"
#include "LODData.h"

using namespace maths;

class PrivateWorld {
public:
    PrivateWorld() {}

	std::map<int, std::unique_ptr<LODData>> _lodData;
	std::map<ChunkPosition, std::unique_ptr<Chunk>> _chunks;

	std::vector<std::unique_ptr<IWorldExpander>> _expanders;
	std::vector<std::unique_ptr<IWorldChunkDecorator>> _chunkDecorators;
};


World::World() : _internal(new PrivateWorld()), _directory() {

}

World::~World() {
    delete _internal;
}

void World::setLODData(int lod, const LODData & data) {
	if (_internal->_lodData.find(lod) != _internal->_lodData.end()) {
		throw std::runtime_error("this lod data already exists and can not be modified");
	}

	_internal->_lodData[lod] = std::make_unique<LODData>(data);
}

LODData & World::getLODData(int lod) const {
	return *_internal->_lodData.at(lod);
}

void World::addObject(WorldObject * object) {
	Chunk & chunk = getOrCreateChunk(getChunkPosition(object->getPosition()));
	chunk.addObject(object);
}

void World::addExpander(IWorldExpander * expander) {
	_internal->_expanders.emplace_back(expander);
}

void World::addChunkDecorator(IWorldChunkDecorator * decorator) {
	_internal->_chunkDecorators.emplace_back(decorator);
}

bool World::isChunkGenerated(const ChunkPosition & position) {
	auto & chunks = _internal->_chunks;
	return chunks.find(position) != chunks.end();
}

Chunk & World::getChunk(const ChunkPosition & position) const {
	return *_internal->_chunks.at(position); // TODO throw specific exception
}

ChunkPosition World::getChunkPosition(const ObjectPosition & position) {
	const vec3d & pos3D = position.getPosition3D();
	const LODData & lod = getOrCreateLODData(position.getMaxLOD());
	const vec3d & chunkSize = lod.getChunkSize();
	
	return ChunkPosition(
		(int) floor(pos3D.x / chunkSize.x),
		(int) floor(pos3D.y / chunkSize.y),
		(int) floor(pos3D.z / chunkSize.z),
		position.getMaxLOD());
}

void World::expand(const IPointOfView & from) {
	for (auto & expander : _internal->_expanders) {
		expander->expand(*this, from);
	}

	LODData & lodData = getOrCreateLODData(1);

	iterateChunkPosInSight(from, lodData, [&](const ChunkPosition & chunkPos) {
		if (!isChunkGenerated(chunkPos)) {
			generateChunk(chunkPos);
		}
	});
}

void World::generateChunk(const ChunkPosition & position) {
	generateChunk(getOrCreateChunk(position));
}

void World::generateChunk(Chunk & chunk) {
	for (auto & decorator : _internal->_chunkDecorators) {
		decorator->decorate(*this, chunk);
	}
}

Scene * World::createSceneFrom(const IPointOfView & from) const {
	Scene * scene = new Scene();

	for (auto & lodPair : _internal->_lodData) {
		LODData & lod = *lodPair.second;

		iterateChunkPosInSight(from, lod, [&](const ChunkPosition & chunkPos) {
			try {
				getChunk(chunkPos).fillScene(*scene);
			}
			catch (std::exception & e) {} 
		});
	}

	return scene;
}

LODData & World::getOrCreateLODData(int lod) {
	auto lodData = _internal->_lodData.find(lod);

	if (lodData == _internal->_lodData.end()) {
		return *(_internal->_lodData[lod] = std::make_unique<LODData>(vec3d(1000, 1000, 1000)));
	}
	else {
		return *(*lodData).second;
	}
}

Chunk & World::getOrCreateChunk(const ChunkPosition & position) {
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

void World::iterateChunkPosInSight(const IPointOfView & from, const LODData & lodData, const std::function<void(const ChunkPosition&)> & action) const {
	vec3d chunkSize = lodData.getChunkSize();

	vec3d center = from.getPosition();
	double horizon = from.getHorizonDistance();
	vec3i centerChunk = center / chunkSize;
	vec3i hsize = vec3d(horizon, horizon, horizon) / chunkSize;

	for (int x = centerChunk.x - hsize.x - 1; x < centerChunk.x + hsize.x + 1; x++) {
		for (int y = centerChunk.y - hsize.y - 1; y < centerChunk.y + hsize.y + 1; y++) {
			ChunkPosition chunkPos(x, y, 0, 1);

			action(chunkPos);
		}
	}
}
