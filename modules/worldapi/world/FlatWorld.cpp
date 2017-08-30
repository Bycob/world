#include "FlatWorld.h"

class PrivateFlatWorld {
public:
	PrivateFlatWorld() {}

	std::vector<std::unique_ptr<IFlatWorldExpander>> _expanders;
};

FlatWorld::FlatWorld() : _internal(new PrivateFlatWorld()) {

}

FlatWorld::~FlatWorld() {
	delete _internal;
}

void FlatWorld::expand(const IPointOfView & from) {
	World::expand(from);

	for (auto & expander : _internal->_expanders) {
		expander->expand(*this, from);
	}
}

void FlatWorld::addFlatWorldExpander(IFlatWorldExpander * expander) {
	_internal->_expanders.emplace_back(expander);
}