#ifndef WORLD_IEXPLORER_H
#define WORLD_IEXPLORER_H

#include "world/core/WorldConfig.h"

#include "ExplorationContext.h"
#include "world/math/MathsHelper.h"
#include "world/math/Vector.h"
#include "world/math/BoundingBox.h"

namespace world {

class WorldZone;

class WORLDAPI_EXPORT IResolutionModel {
public:
    virtual ~IResolutionModel() = default;

    virtual double getResolutionAt(const vec3d &coord) const = 0;

    double getResolutionAt(const vec3d &coord,
                           const ExplorationContext &ctx) const {
        return getResolutionAt(coord + ctx.getOffset());
    }

    virtual double getMaxResolutionIn(const BoundingBox &bbox) const = 0;

    double getMaxResolutionIn(const BoundingBox &bbox,
                              const ExplorationContext &ctx) const {
        BoundingBox bbox2 = bbox;
        bbox2.translate(ctx.getOffset());
        return getMaxResolutionIn(bbox2);
    }

    /** Bounds of the non-zero resolution zone. Everything outside of
     * this box has a resolution of 0. */
    virtual BoundingBox getBounds() const = 0;

    BoundingBox getBounds(const ExplorationContext &ctx) {
        BoundingBox bounds = getBounds();
        bounds.translate(-ctx.getOffset());
        return bounds;
    }

private:
};

} // namespace world

#endif // WORLD_IEXPLORER_H
