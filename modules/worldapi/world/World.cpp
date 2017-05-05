#include "World.h"

#include "WorldGenerator.h"

class PrivateWorld {
public:
    template <typename... Args>
    PrivateWorld(Args... args)
            : _gen(std::make_unique<WorldGenerator>(args...)) {}

    std::unique_ptr<WorldGenerator> _gen;
	std::vector<std::unique_ptr<WorldNode>> _nodes;
};


World::World() : _internal(new PrivateWorld()), _directory() {
    getGenerator().init(*this);
}

World::World(const WorldGenerator &generator)
	: _internal(new PrivateWorld(generator)), _directory() {
    getGenerator().init(*this);
}

World::~World() {
    delete _internal;
}

WorldGenerator& World::getGenerator() {
    return *_internal->_gen;
}

std::vector<std::unique_ptr<WorldNode>> & World::_nodes() {
	return _internal->_nodes;
}

bool World::checkNodeTypeInternal(const WorldNodeType &type) const {
    if (type.unique()) {
        for (const std::unique_ptr<WorldNode> & node : _internal->_nodes) {
            if (node->type() == type) {
                return false;
            }
        }
    }

    return true;
}
