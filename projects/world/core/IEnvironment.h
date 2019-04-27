#ifndef WORLD_IENVIRONMENT_H
#define WORLD_IENVIRONMENT_H

#include "world/core/WorldConfig.h"

#include "world/math/Vector.h"

namespace world {

class ExplorationContext;

class WORLDAPI_EXPORT IEnvironment {
public:
    virtual ~IEnvironment() = default;

    virtual vec3d findNearestFreePoint(const vec3d &origin,
                                       const vec3d &direction,
                                       double resolution,
                                       const ExplorationContext &ctx) const = 0;
};

} // namespace world

#endif // WORLD_IENVIRONMENT_H
