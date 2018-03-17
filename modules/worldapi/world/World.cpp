#include "World.h"

#include <map>

#include "FlatWorld.h"
#include "SimpleTreeDecorator.h"
#include "Chunk.h"
#include "LODData.h"

namespace world {

	class PrivateWorld {
	public:
		PrivateWorld() = default;

		std::vector<std::unique_ptr<IWorldChunkDecorator>> _chunkDecorators;
	};


	World *World::createDemoWorld() {
		FlatWorld *world = new FlatWorld();

		world->addFlatWorldDecorator(new SimpleTreeDecorator(1));

		return world;
	}


	World::World() : _internal(new PrivateWorld()), _directory() {

	}

	World::~World() {
		delete _internal;
	}

	void World::addChunkDecorator(IWorldChunkDecorator *decorator) {
		_internal->_chunkDecorators.emplace_back(decorator);
	}

	WorldZone World::exploreLocation(const vec3d &location) {
		auto pair = _chunkSystem.getOrCreateZone(location);
		WorldZone &chunk = pair.first;

		if (pair.second) {
			onFirstExploration(chunk);
		}
		return pair.first;
	}

	WorldZone World::exploreNeighbour(const WorldZone &zone, const vec3d &direction) {
		auto pair = zone->getOrCreateNeighbourZone(direction);
		WorldZone &nchunk = pair.first;

		if (pair.second) {
			onFirstExploration(nchunk);
		}
		return pair.first;
	}

	std::vector<WorldZone> World::exploreInside(const WorldZone &zone) {
		auto zones = zone->getOrCreateChildren();
		std::vector<WorldZone> result;

		for (auto &pair : zones) {
			if (pair.second) {
				onFirstExploration(pair.first);
			}

			result.emplace_back(pair.first);
		}

		return result;
	}

	void World::onFirstExploration(WorldZone &chunk) {
		for (auto &decorator : _internal->_chunkDecorators) {
			decorator->decorate(*this, chunk);
		}
	}
}
