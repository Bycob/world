#ifndef WORLD_IENVIRONMENT_H
#define WORLD_IENVIRONMENT_H

#include "world/core/WorldConfig.h"

#include "world/math/Vector.h"

namespace world {

class ExplorationContext;

struct AtmosphericData {
    /// in percentage
    double _humidity;
    /// in celsius degrees
    double _temperature;
};


class WORLDAPI_EXPORT IAtmosphericProvider {
public:
    virtual ~IAtmosphericProvider() = default;

    virtual AtmosphericData getAtmosphericDataPoint(
        const vec3d &point, double resolution,
        const ExplorationContext &ctx) = 0;
};


class WORLDAPI_EXPORT IEnvironment {
public:
    virtual ~IEnvironment() = default;

    virtual vec3d findNearestFreePoint(const vec3d &origin,
                                       const vec3d &direction,
                                       double resolution,
                                       const ExplorationContext &ctx) const = 0;

    virtual AtmosphericData getAtmosphericDataPoint(
        const vec3d &point, double resolution,
        const ExplorationContext &ctx) const = 0;

    virtual void setAtmosphericProvider(
        IAtmosphericProvider *provider) const = 0;
};

class WORLDAPI_EXPORT DefaultEnvironment : public IEnvironment {
public:
    /** Definition in ExplorationContext.cpp */
    static DefaultEnvironment &getDefault();

    vec3d findNearestFreePoint(const vec3d &origin, const vec3d &direction,
                               double resolution,
                               const ExplorationContext &ctx) const override {
        vec3d res = origin;
        res.z = 0;
        return res;
    }

    AtmosphericData getAtmosphericDataPoint(
        const vec3d &point, double resolution,
        const ExplorationContext &ctx) const override {

        if (_atmosphericProvider != nullptr) {
            return _atmosphericProvider->getAtmosphericDataPoint(
                point, resolution, ctx);
        }
        return {0.5, 15};
    }

    void setAtmosphericProvider(IAtmosphericProvider *provider) const override {
        _atmosphericProvider = provider;
    }

protected:
    mutable IAtmosphericProvider *_atmosphericProvider = nullptr;
};

} // namespace world

#endif // WORLD_IENVIRONMENT_H
