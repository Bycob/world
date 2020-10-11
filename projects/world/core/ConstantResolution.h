#ifndef WORLD_CONSTANTRESOLUTION_H
#define WORLD_CONSTANTRESOLUTION_H

#include "world/core/WorldConfig.h"

#include "IResolutionModel.h"

namespace world {

class WORLDAPI_EXPORT ConstantResolution : public IResolutionModel {
public:
    ConstantResolution(double resolution)
            : _bbox{{-10000, -10000, -10000}, {10000, 10000, 10000}},
              _resolution(resolution) {}

    void setResolution(double resolution) { _resolution = resolution; }

    double getResolution() const { return _resolution; }

    double getResolutionAt(const vec3d &coord) const override {
        return _bbox.contains(coord) ? _resolution : 0;
    }

    double getMaxResolutionIn(const BoundingBox &bbox) const override {
        // TODO bbox intersects
        return _resolution;
    }

    void setBounds(const BoundingBox &bbox) { _bbox = bbox; }

    BoundingBox getBounds() const override { return _bbox; }

private:
    BoundingBox _bbox;
    double _resolution;
};

} // namespace world

#endif // WORLD_CONSTANTRESOLUTION_H
