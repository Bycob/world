#ifndef WORLD_IGROUND_H
#define WORLD_IGROUND_H

#include "world/core/WorldConfig.h"

#include "world/core/WorldNode.h"
#include "world/assets/Image.h"

namespace world {

class FlatWorld;

class WORLDAPI_EXPORT IGround {
public:
    virtual ~IGround() = default;

    virtual double observeAltitudeAt(double x, double y, double resolution) = 0;

    virtual void paintTexture(const vec2d &origin, const vec2d &size,
                              const vec2d &resolutionRange,
                              const Image &img) = 0;
};

class WORLDAPI_EXPORT GroundNode : public WorldNode, public IGround {};

} // namespace world

#endif // WORLD_IGROUND_H
