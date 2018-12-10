#include "WorldObject.h"

#include <stdexcept>
#include <iostream>

#include "LODGridChunkSystem.h"
#include "Collector.h"
#include "IResolutionModel.h"
#include "world/core/CollectorContextWrap.h"
#include "world/core/ResolutionModelContextWrap.h"

namespace world {

class ConstantResolution : public IResolutionModel {
public:
    double _resolution;

    ConstantResolution(double resolution) : _resolution(resolution) {}

    double getResolutionAt(const vec3d &coord) const override {
        return _resolution;
    }

    double getResolutionAt(const WorldZone &zone,
                           const vec3d &coord) const override {
        return _resolution;
    }

    double getMaxResolutionIn(const BoundingBox &bbox) const override {
        return _resolution;
    }
};


WorldObject::WorldObject() {}

WorldObject::~WorldObject() {}

void WorldObject::setPosition3D(const vec3d &pos) { _position = pos; }

void WorldObject::collectAll(ICollector &collector, double resolution) {
    collect(collector, ConstantResolution(resolution));
}

void WorldObject::collect(ICollector &collector,
                          const IResolutionModel &resolutionModel) {}

void WorldObject::collectChild(int keyOffset, WorldObject &childObject,
                               ICollector &collector,
                               const IResolutionModel &resolutionModel) {
    CollectorContextWrap wcollector(collector);
    wcollector.setKeyOffset(keyOffset);
    wcollector.setOffset(childObject.getPosition3D());

    ResolutionModelContextWrap wresolutionModel(resolutionModel);
    wresolutionModel.setOffset(childObject.getPosition3D());

    childObject.collect(wcollector, wresolutionModel);
}

} // namespace world
