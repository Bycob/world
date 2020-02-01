#include "WorldNode.h"
#include "WorldNode_p.h"

#include <stdexcept>
#include <iostream>

#include "GridChunkSystem.h"
#include "Collector.h"
#include "IResolutionModel.h"
#include "ConstantResolution.h"

namespace world {

WorldNode::WorldNode() : _internal(new WorldNodePrivate()) {}

WorldNode::~WorldNode() { delete _internal; }

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

void WorldNode::addChildInternal(WorldNode *node) {
    NodeKey key = NodeKeys::fromInt(_internal->_counter);
    _internal->_children.emplace(key, std::unique_ptr<WorldNode>(node));
    node->_key = key;
    _internal->_counter++;
}
} // namespace world
