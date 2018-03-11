#include "FlatWorld.h"

class PrivateFlatWorld {
public:
	PrivateFlatWorld() {}

	std::vector<std::unique_ptr<IFlatWorldDecorator>> _chunkDecorators;
};

FlatWorld::FlatWorld() : _internal(new PrivateFlatWorld()) {

}

FlatWorld::~FlatWorld() {
	delete _internal;
}

void FlatWorld::addFlatWorldDecorator(IFlatWorldDecorator *decorator) {
	_internal->_chunkDecorators.emplace_back(decorator);
}

Ground& FlatWorld::ground() {
	return _ground;
}

void FlatWorld::onFirstExploration(ChunkNode &chunk) {
    World::onFirstExploration(chunk);

	for (auto & decorator : _internal->_chunkDecorators) {
		decorator->decorate(*this, chunk);
	}
}