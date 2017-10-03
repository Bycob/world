#include "FlatWorld.h"

class PrivateFlatWorld {
public:
	PrivateFlatWorld() {}

	std::vector<std::unique_ptr<IFlatWorldExpander>> _expanders;
	std::vector<std::unique_ptr<IFlatWorldChunkDecorator>> _chunkDecorators;
};

FlatWorld::FlatWorld() : _internal(new PrivateFlatWorld()) {

}

FlatWorld::~FlatWorld() {
	delete _internal;
}

void FlatWorld::addFlatWorldExpander(IFlatWorldExpander * expander) {
	_internal->_expanders.emplace_back(expander);
}

void FlatWorld::addFlatWorldChunkDecorator(IFlatWorldChunkDecorator * decorator) {
	_internal->_chunkDecorators.emplace_back(decorator);
}

void FlatWorld::callExpanders(const IPointOfView &from) {
	World::callExpanders(from);

	for (auto & expander : _internal->_expanders) {
		expander->expand(*this, from);
	}
}

void FlatWorld::generateChunk(Chunk & chunk) {
	World::generateChunk(chunk);

	for (auto & decorator : _internal->_chunkDecorators) {
		decorator->decorate(*this, chunk);
	}
}