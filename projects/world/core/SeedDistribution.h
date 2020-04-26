#ifndef WORLD_SEEDDISTRIBUTION_H
#define WORLD_SEEDDISTRIBUTION_H

#include "world/core/WorldConfig.h"

#include <map>

#include "world/core/WorldTypes.h"
#include "world/core/IEnvironment.h"
#include "world/math/Vector.h"
#include "world/core/Chunk.h"
#include "InstanceDistribution.h"

namespace world {

struct WORLDAPI_EXPORT Seed {
    vec2d _position;
    u32 _generatorId;
    double _distance = 1000;
};

class WORLDAPI_EXPORT SeedDistribution : public DistributionBase {
public:
    SeedDistribution() : DistributionBase() {
        _tileSize = _maxDist;
        // 0.75 = Expected value of law 1 - X^2 with X in [0, 1]
        double invMeanRadius = 1 / (0.75 / 1000 * _maxDist);
        _seedDensity = 1 / M_PI * invMeanRadius * invMeanRadius * _seedAmount;
    }

    void addSeeds(Chunk &chunk);

    std::vector<Seed> getSeedsAround(Chunk &chunk);

    std::vector<Position> getPositions(Chunk &chunk,
                                       const ExplorationContext &ctx);

private:
    double _tileSize;
    /// Number of seeds per km^2. Computed from seedAmount and maxDist.
    double _seedDensity;
    std::map<vec2i, std::vector<Seed>> _seeds;

    /// Mean amount of seeds occupying the same territory.
    double _seedAmount = 2;
    /// Maximum distance a seed can spread on
    double _maxDist = 2000;


    /** Returns id bounds of the zone around the given chunk */
    std::pair<vec2i, vec2i> getBounds(const Chunk &chunk) const;
};

} // namespace world

#endif // WORLD_SEEDDISTRIBUTION_H
