#include "World.h"

#include <map>

#include "world/flat/FlatWorld.h"
#include "GridChunkSystem.h"

namespace world {

class WorldPrivate {
public:
    WorldPrivate() = default;

    int _counter = 0;
    std::map<NodeKey, std::unique_ptr<WorldNode>> _primaryNodes;
};


World *World::createDemoWorld() { return FlatWorld::createDemoFlatWorld(); }


World::World() : _internal(new WorldPrivate()) {}

World::~World() { delete _internal; }

void World::collect(ICollector &collector,
                    const IResolutionModel &resolutionModel) {

    for (auto &entry : _internal->_primaryNodes) {
        ExplorationContext ctx;
        ctx.setEnvironment(getInitialEnvironment());
        ctx.appendPrefix(entry.first);
        ctx.addOffset(entry.second->getPosition3D());

        entry.second->collect(collector, resolutionModel, ctx);
    }
}

void World::write(WorldFile &wf) const {
    wf.addArray("nodes");

    for (auto &entry : _internal->_primaryNodes) {
        WorldFile nodeFile = entry.second->serializeSubclass();
        wf.addToArray("nodes", std::move(nodeFile));
    }
}

void World::read(const WorldFile &wf) {
    for (auto it = wf.readArray("nodes"); !it.end(); ++it) {
        std::unique_ptr<WorldNode> node(readSubclass<WorldNode>(*it));
        _internal->_primaryNodes.emplace(node->getKey(), std::move(node));
    }
}

void World::addPrimaryNodeInternal(WorldNode *node) {
    if (_internal->_counter > MAX_PRIMARY_NODES) {
        throw std::runtime_error(
            "World is not meant to have so many primary nodes! Consider using "
            "a chunksystem instead.");
    }

    auto it = _internal->_primaryNodes
                  .emplace(std::to_string(++_internal->_counter),
                           std::unique_ptr<WorldNode>(node))
                  .first;
    it->second->setKey(it->first);
    it->second->configureCache(_cacheRoot, it->first);
    // But we have to set the key still, we cannot ignore that problem :(
}

IEnvironment *World::getInitialEnvironment() { return nullptr; }

} // namespace world
