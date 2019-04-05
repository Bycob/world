#include "WorldNode.h"
#include "WorldNode_p.h"

#include <stdexcept>
#include <iostream>

#include "LODGridChunkSystem.h"
#include "Collector.h"
#include "IResolutionModel.h"

namespace world {

class ConstantResolution : public IResolutionModel {
public:
    double _resolution;

    ConstantResolution(double resolution) : _resolution(resolution) {}

    double getResolutionAt(const vec3d &coord) const override {
        return _resolution;
    }

    double getMaxResolutionIn(const BoundingBox &bbox) const override {
        return _resolution;
    }

    // TODO set a more precise bbox
    BoundingBox getBounds() const override {
        return {{-10000, -10000, -10000}, {10000, 10000, 10000}};
    }
};


WorldNode::WorldNode() : _internal(new WorldNodePrivate()) {}

WorldNode::~WorldNode() { delete _internal; }

void WorldNode::setPosition3D(const vec3d &pos) { _position = pos; }

void WorldNode::collectAll(ICollector &collector, double resolution) {
    collect(collector, ConstantResolution(resolution));
}

void WorldNode::collect(ICollector &collector,
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
