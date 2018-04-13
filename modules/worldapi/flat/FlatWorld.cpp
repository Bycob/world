#include "FlatWorld.h"

#include "terrain/Ground.h"
#include "SimpleTreeDecorator.h"
#include "FlatWorldCollector.h"

namespace world {

	class PrivateFlatWorld {
	public:
		PrivateFlatWorld() {}

		std::vector<std::unique_ptr<FlatWorldDecorator>> _chunkDecorators;
	};

	FlatWorld* FlatWorld::createDemoFlatWorld() {
		FlatWorld *world = new FlatWorld();

		world->addFlatWorldDecorator(new SimpleTreeDecorator(3));

		return world;
	}

	FlatWorld::FlatWorld()
			: _internal(new PrivateFlatWorld()) {
		setGround<Ground>();
	}

	FlatWorld::~FlatWorld() {
		delete _internal;
	}

	IGround &FlatWorld::ground() {
		return *_ground;
	}

	void FlatWorld::addFlatWorldDecorator(FlatWorldDecorator *decorator) {
		_internal->_chunkDecorators.emplace_back(decorator);
	}

	void world::FlatWorld::collect(const WorldZone & zone, ICollector & collector) {
		World::collect(zone, collector);

		ground().collectZone(zone, collector);
	}

	void FlatWorld::collect(const WorldZone &zone, FlatWorldCollector &collector) {
		World::collect(zone, collector);

		ground().collectZone(zone, *this, collector);
	}

	void FlatWorld::onFirstExploration(WorldZone &chunk) {
		World::onFirstExploration(chunk);

		for (auto &decorator : _internal->_chunkDecorators) {
			decorator->decorate(*this, chunk);
		}
	}

	void FlatWorld::setGroundInternal(IGround *ground) {
		_ground = std::unique_ptr<IGround>(ground);
	}
}