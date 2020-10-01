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

    virtual double observeAltitudeAt(double x, double y, double resolution,
                                     const ExplorationContext &ctx) = 0;

    /** Paint the given image on the terrain texture.
     * \param origin the (x, y) coordinates of the top left corner of
     * the image on the terrain, in meters.
     * \param size the size of the image on the terrain, in meters
     * \param resolutionRange */
    virtual void paintTexture(const vec2d &origin, const vec2d &size,
                              const vec2d &resolutionRange, const Image &img,
                              const ExplorationContext &ctx) = 0;
};

class WORLDAPI_EXPORT GroundNode : public WorldNode, public IGround {};

} // namespace world

#endif // WORLD_IGROUND_H
