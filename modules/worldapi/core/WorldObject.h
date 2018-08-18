#pragma once

#include "core/WorldConfig.h"

#include "math/Vector.h"
#include "math/MathsHelper.h"
#include "ICollector.h"
#include "IResolutionModel.h"

namespace world {

class WORLDAPI_EXPORT WorldObject {
public:
    WorldObject();

    virtual ~WorldObject();

    void setPosition3D(const vec3d &pos);

    vec3d getPosition3D() const { return _position; }

    void collectAll(ICollector &collector, double resolution);

    virtual void collect(ICollector &collector,
                         const IResolutionModel &resolutionModel);

protected:
    vec3d _position;

	void collectChild(int keyOffset, WorldObject &childObject, ICollector &collector, const IResolutionModel &resolutionModel);
};
} // namespace world