#include "WorldNode.h"
#include "WorldNode_p.h"

#include <stdexcept>
#include <iostream>

#include "LODGridChunkSystem.h"
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

void WorldNode::addChildInternal(WorldNode *node) {
    _internal->_children.emplace(NodeKeys::fromInt(_internal->_counter),
                                 std::unique_ptr<WorldNode>(node));
    _internal->_counter++;
}
} // namespace world
