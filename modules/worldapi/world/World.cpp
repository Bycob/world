#include "World.h"

#include "WorldGenerator.h"

class InternalWorldGenerator {
public:
    template <typename... Args>
    InternalWorldGenerator(Args... args)
            : _gen(std::make_unique<WorldGenerator>(args...)) {}

    std::unique_ptr<WorldGenerator> _gen;
};


World::World() : _internal(new InternalWorldGenerator()) {
    getGenerator().init(*this);
}

World::World(const WorldGenerator &generator)
        : _internal(new InternalWorldGenerator(generator)) {
    getGenerator().init(*this);
}

World::~World() {
    delete _internal;
}

WorldGenerator& World::getGenerator() {
    return *_internal->_gen;
}

bool World::checkNodeTypeInternal(const WorldNodeType &type) const {
    if (type.unique()) {
        for (const std::unique_ptr<WorldNode> & node : _nodes) {
            if (node->type() == type) {
                return false;
            }
        }
    }

    return true;
}
