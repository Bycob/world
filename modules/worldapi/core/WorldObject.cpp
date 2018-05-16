#include "WorldObject.h"

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

    double getResolutionAt(const WorldZone &zone,
                           const vec3d &coord) const override {
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

} // namespace world
