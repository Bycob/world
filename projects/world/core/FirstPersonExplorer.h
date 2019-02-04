#ifndef WORLD_FIRSTPERSONWORLDEXPLORER_H
#define WORLD_FIRSTPERSONWORLDEXPLORER_H

#include "world/core/WorldConfig.h"

#include "IResolutionModel.h"
#include "World.h"
#include "ICollector.h"
#include "ExplorationResult.h"

namespace world {

class WORLDAPI_EXPORT FirstPersonExplorer : public IResolutionModel {
public:
    /** Build a FirstPersonExplorer with the given parameters.
     * @param eyeResolution the resolution of our eye
     * @param fov the field of view
     * @param punctumProximum The minimum distance under which we
     * cannot see. */
    FirstPersonExplorer(double eyeResolution = 1000, double fov = 90,
                        double punctumProximum = 1);

    void setPosition(const vec3d &position);

    void setEyeResolution(double resolution);

    void setFOV(double fov);

    void setPunctumProximum(double punctumProximum);

    void setFarDistance(double maxDistance);

    double getResolutionAt(const vec3d &pos) const override;

    double getResolutionAt(const WorldZone &zone,
                           const vec3d &pos) const override;

    double getMaxResolutionIn(const BoundingBox &bbox) const override;

    // TODO std::enableif
    template <typename T, typename C>
    void exploreAndCollect(T &world, C &collector);

    void explore(World &world, ExplorationResult &result);

private:
    double _eyeResolution;
    double _fov;
    double _punctumProximum;
    /** The maximum distance the explorer can see over. In real world,
     * we cannot see farther than a certain distance due to mist or Earth
     * curvature, for example. */
    double _farDistance;
    vec3d _position;

    vec3d getNearestPointIn(const BoundingBox &bbox) const;

    vec3d getChunkNearestPoint(const WorldZone &zone) const;

    void exploreVertical(World &world, const WorldZone &zone,
                         ExplorationResult &result);
};
} // namespace world

#include "FirstPersonExplorer.inl"

#endif // WORLD_FIRSTPERSONWORLDEXPLORER_H