
#ifndef WORLD_EXPLORER_CONTEXT_WRAP_H
#define WORLD_EXPLORER_CONTEXT_WRAP_H

#include "core/WorldConfig.h"

#include "IResolutionModel.h"
#include "math/MathsHelper.h"

namespace world {

class WORLDAPI_EXPORT ResolutionModelContextWrap : public IResolutionModel {
public:
    ResolutionModelContextWrap(const IResolutionModel &resolutionModel);

    void setOffset(const vec3d &offset);

    double getResolutionAt(const vec3d &coord) const override;

    double getResolutionAt(const WorldZone &zone,
                           const vec3d &coord) const override;

    double getMaxResolutionIn(const BoundingBox &bbox) const override;

private:
    const IResolutionModel &_wrapped;

    vec3d _offset;
};

} // namespace world

#endif // WORLD_EXPLORER_CONTEXT_WRAP_H
