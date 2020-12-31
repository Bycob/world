#include "WorldNode.h"
#include "WorldNode_p.h"

#include <stdexcept>
#include <iostream>

#include "GridChunkSystem.h"
#include "Collector.h"
#include "IResolutionModel.h"
#include "ConstantResolution.h"

namespace world {

WORLD_REGISTER_BASE_CLASS(WorldNode)

WorldNode::WorldNode() : _internal(new WorldNodePrivate()) {}

WorldNode::~WorldNode() { delete _internal; }

void WorldNode::setKey(NodeKey key) { _key = std::move(key); }

void WorldNode::configureCache(NodeCache &parent) {
    _cache.setChild(parent, _key);
}

void WorldNode::setPosition3D(const vec3d &pos) { _position = pos; }

void WorldNode::collectAll(ICollector &collector, double resolution) {
    collect(collector, ConstantResolution(resolution));
}

void WorldNode::collect(ICollector &collector,
                        const IResolutionModel &resolutionModel,
                        const ExplorationContext &ctx) {

    collectSelf(collector, resolutionModel, ctx);
    collectChildren(collector, resolutionModel, ctx);
}

void WorldNode::collectSelf(ICollector &collector,
                            const IResolutionModel &resolutionModel,
                            const ExplorationContext &ctx) {}

void WorldNode::collectChildren(ICollector &collector,
                                const IResolutionModel &resolutionModel,
                                const ExplorationContext &ctx) {

    for (auto &entry : _internal->_children) {
        collectChild(entry.first, *entry.second, collector, resolutionModel,
                     ctx);
    }
}

void WorldNode::collectChild(const NodeKey &key, WorldNode &childObject,
                             ICollector &collector,
                             const IResolutionModel &resolutionModel,
                             const ExplorationContext &ctx) {
    ExplorationContext newCtx = ctx;
    newCtx.appendPrefix(key);
    newCtx.addOffset(childObject.getPosition3D());

    childObject.collect(collector, resolutionModel, newCtx);
}

void WorldNode::removeChild(WorldNode &child) {
    _internal->_children.erase(child._key);
}

void WorldNode::write(WorldFile &wf) const {
    wf.addString("key", NodeKeys::toString(_key));
    wf.addStruct("position", _position);
    wf.addArray("children");

    for (auto &p : _internal->_children) {
        wf.addToArray("children", p.second->serializeSubclass());
    }
}

void WorldNode::read(const WorldFile &wf) {
    // This key is overriden later, so this line is technically useless :)
    _key = NodeKeys::fromString(wf.readString("key"));
    wf.readStruct("position", _position);

    for (auto it = wf.readArray("children"); !it.end(); ++it) {
        // We don't keep the serialized key (maybe we should?)
        addChildInternal(readSubclass<WorldNode>(*it));
    }
}

void WorldNode::addChildInternal(WorldNode *node) {
    NodeKey key = NodeKeys::fromInt(_internal->_counter);
    _internal->_children.emplace(key, std::unique_ptr<WorldNode>(node));
    node->_key = key;
    node->_cache.setChild(_cache, key);
    _internal->_counter++;
}
} // namespace world
