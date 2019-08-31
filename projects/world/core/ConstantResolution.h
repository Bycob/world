#ifndef WORLD_CONSTANTRESOLUTION_H
#define WORLD_CONSTANTRESOLUTION_H

#include "world/core/WorldConfig.h"

#include "IResolutionModel.h"

namespace world {

class WORLDAPI_EXPORT ConstantResolution : public IResolutionModel {
public:
    ConstantResolution(double resolution)
            : _resolution(resolution), _bbox{{-10000, -10000, -10000},
                                             {10000, 10000, 10000}} {}

    void setResolution(double resolution) { _resolution = resolution; }

    double getResolution() const { return _resolution; }

    double getResolutionAt(const vec3d &coord) const override {
        return _bbox.contains(coord) ? _resolution : 0;
    }

    double getMaxResolutionIn(const BoundingBox &bbox) const override {
        // TODO bbox intersects
        return _resolution;
    }

    BoundingBox getBounds() const override { return _bbox; }

private:
    double _resolution;
    BoundingBox _bbox;
};

} // namespace world

#endif // WORLD_CONSTANTRESOLUTION_H
