#ifndef WORLD_SEEDDISTRIBUTION_H
#define WORLD_SEEDDISTRIBUTION_H

#include "world/core/WorldConfig.h"

#include <map>

#include "world/core/WorldTypes.h"
#include "world/core/IEnvironment.h"
#include "world/math/Vector.h"
#include "world/core/Chunk.h"

namespace world {

/** Habitat features for a given "species" of object. */
struct WORLDAPI_EXPORT HabitatFeatures {
    vec2d _altitude{0, 2000};
    // radians
    vec2d _slope{0, M_PI_2};
    // celsius
    vec2d _temperature{0, 40};
    vec2d _humidity{0, 1};
    bool _sea = false;
    double _density = 0.1;

    // TODO add physical materials
};

struct WORLDAPI_EXPORT Seed {
    vec2d _position;
    u32 _generatorId;
    double _distance = 1000;
};

class WORLDAPI_EXPORT SeedDistribution {
public:
    SeedDistribution(IEnvironment *env)
            : _env(env), _rng{static_cast<u32>(time(NULL))} {
        _tileSize = _maxDist;
        // 0.75 = Expected value of law 1 - X^2 with X in [0, 1]
        double invMeanRadius = 1 / (0.75 * _maxDist);
        _seedDensity = 1 / M_PI * invMeanRadius * invMeanRadius * _seedAmount;
    }

    // Add seeds
    void addSeeds(Chunk &chunk);

    std::vector<Seed> getSeedsAround(Chunk &chunk);

    std::vector<vec3d> getPositions(Chunk &chunk, int generatorId);

private:
    IEnvironment *_env;
    std::mt19937 _rng;

    double _tileSize;
    /// Number of seeds per km^2. Computed from seedAmount and maxDist.
    double _seedDensity;
    std::map<vec2i, std::vector<Seed>> _seeds;
    std::map<int, HabitatFeatures> _habitats;

    double _resolution = 20;

    /// Mean amount of seeds occupying the same territory.
    double _seedAmount = 2;
    /// Maximum distance a seed can
    double _maxDist = 2000;


    /** Returns id bounds of the zone around the given chunk */
    std::pair<vec2i, vec2i> getBounds(const Chunk &chunk) const;
};

} // namespace world

#endif // WORLD_SEEDDISTRIBUTION_H
