#ifndef WORLD_FIRSTPERSONWORLDEXPLORER_H
#define WORLD_FIRSTPERSONWORLDEXPLORER_H

#include "world/core/WorldConfig.h"

#include "IResolutionModel.h"
#include "World.h"
#include "ICollector.h"

namespace world {

class WORLDAPI_EXPORT FirstPersonView : public IResolutionModel {
public:
    /** Build a FirstPersonView with the given parameters.
     * @param eyeResolution the resolution of our eye
     * @param fov the field of view
     * @param punctumProximum The minimum distance under which we
     * cannot see. */
    FirstPersonView(double eyeResolution = 1000, double fov = 90,
                    double punctumProximum = 1);

    void setPosition(const vec3d &position);

    void setEyeResolution(double resolution);

    void setFOV(double fov);

    void setPunctumProximum(double punctumProximum);

    void setFarDistance(double maxDistance);

    vec3d getNearestPointIn(const BoundingBox &bbox) const;

    double getResolutionAt(const vec3d &pos) const override;

    double getMaxResolutionIn(const BoundingBox &bbox) const override;

    BoundingBox getBounds() const override;

private:
    double _eyeResolution;
    double _fov;
    double _punctumProximum;
    /** The maximum distance the explorer can see over. In real world,
     * we cannot see farther than a certain distance due to mist or Earth
     * curvature, for example. */
    double _farDistance;
    vec3d _position;
};
} // namespace world

#endif // WORLD_FIRSTPERSONWORLDEXPLORER_H
