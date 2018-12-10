#ifndef WORLD_IEXPLORER_H
#define WORLD_IEXPLORER_H

#include "world/core/WorldConfig.h"

#include "world/math/MathsHelper.h"
#include "world/math/Vector.h"
#include "world/math/BoundingBox.h"

namespace world {

class WorldZone;

class WORLDAPI_EXPORT IResolutionModel {
public:
    virtual ~IResolutionModel() = default;

    virtual double getResolutionAt(const vec3d &coord) const = 0;

    virtual double getResolutionAt(const WorldZone &zone,
                                   const vec3d &coord) const = 0;

    virtual double getMaxResolutionIn(const BoundingBox &bbox) const = 0;

private:
};

} // namespace world

#endif // WORLD_IEXPLORER_H
