#ifndef WORLD_INSTANCEDISTRIBUTION_H
#define WORLD_INSTANCEDISTRIBUTION_H

#include "world/core/WorldConfig.h"

#include "world/core/Chunk.h"

namespace world {

/** Habitat features for a given "species" of object. */
struct WORLDAPI_EXPORT HabitatFeatures {
    // in m
    vec2d _altitude{0, 2000};
    // radians
    vec2d _slope{0, M_PI_2};
    // celsius
    vec2d _temperature{0, 40};
    vec2d _humidity{0, 1};
    bool _sea = false;
    /// Density per m^2
    double _density = 0.4;

    // TODO add physical materials
};


class WORLDAPI_EXPORT DistributionBase {
public:
    DistributionBase(IEnvironment *env)
            : _env{env}, _rng{static_cast<u32>(time(NULL))} {}

    void setResolution(double resolution) { _resolution = resolution; }

    void addGenerator(HabitatFeatures habitat) {
        _habitats.emplace_back(std::move(habitat));
    }

protected:
    IEnvironment *_env;
    std::mt19937 _rng;

    std::vector<HabitatFeatures> _habitats;
    double _resolution = 20;
};


class WORLDAPI_EXPORT RandomDistribution : public DistributionBase {
public:
    RandomDistribution(IEnvironment *env) : DistributionBase(env) {}

    void setDensity(double density) { _density = density; }

    std::vector<vec3d> getPositions(Chunk &chunk, int generatorId) {
        std::vector<vec3d> positions;

        const vec3d chunkPos = chunk.getPosition3D();
        const vec3d chunkDims = chunk.getSize();

        double chunkArea = chunkDims.x * chunkDims.y;
        int instanceCount = static_cast<int>(floor(chunkArea * _density));
        std::uniform_real_distribution<double> posDistrib(0, 1);

        for (int i = 0; i < instanceCount; ++i) {
            double x = posDistrib(_rng) * chunkDims.x;
            double y = posDistrib(_rng) * chunkDims.y;
            vec3d position =
                _env->findNearestFreePoint(chunkPos + vec3d{x, y, -3000},
                                           vec3d{0, 0, 1}, _resolution,
                                           ExplorationContext::getDefault()) -
                chunkPos;

            if (position.z < 0 || position.z >= chunkDims.z) {
                continue;
            }

            positions.push_back(position);
        }

        return positions;
    }

private:
    // instance count per m^2
    double _density = 0.2;
};

} // namespace world

#endif // WORLD_INSTANCEDISTRIBUTION_H
