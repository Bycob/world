#include "World.h"

#include <map>

#include "CollectorContextWrap.h"
#include "flat/FlatWorld.h"
#include "LODGridChunkSystem.h"

namespace world {

	class PrivateWorld {
	public:
		PrivateWorld() = default;

		std::vector<std::unique_ptr<WorldDecorator>> _chunkDecorators;
	};


	World *World::createDemoWorld() {
		return FlatWorld::createDemoFlatWorld();
	}


	World::World() : _internal(new PrivateWorld()),
                     _chunkSystem(std::make_unique<LODGridChunkSystem>()),
                     _directory() {

	}

	World::~World() {
		delete _internal;
	}

	WorldZone World::exploreLocation(const vec3d &location) {
		auto result = _chunkSystem->getChunk(location);
		WorldZone &zone = result._zone;

		if (result._created) {
			onFirstExploration(zone);
		}
		return result._zone;
	}

	WorldZone World::exploreNeighbour(const WorldZone &zone, const vec3d &direction) {
		auto pair = _chunkSystem->getNeighbourChunk(zone, direction);
		WorldZone &nzone = pair._zone;

		if (pair._created) {
			onFirstExploration(nzone);
		}
		return pair._zone;
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
}
