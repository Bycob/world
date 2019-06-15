#ifndef VKWORLD_DIAMONDSQUARETERRAIN_H
#define VKWORLD_DIAMONDSQUARETERRAIN_H

#include "world/core/WorldConfig.h"

#include "ITerrainWorker.h"

namespace world {

class WORLDAPI_EXPORT DiamondSquareTerrain : public ITerrainWorker {
public:
    /** Create a DiamondSquareTerrain worker.
     * \param jitter the amount of jitter at each stage of the diamond square
     * algorithm, in percentage of the height difference. */
    DiamondSquareTerrain(double jitter = 0.5);

    void processTerrain(Terrain &terrain) override;

    void processTile(ITileContext &context) override;

private:
    std::mt19937_64 _rng;

    std::uniform_real_distribution<double> _jitter;


    double value(double h1, double h2);

    void init(Terrain &terrain);

    void compute(Terrain &terrain, int level);

    void copyParent(const Terrain &parent, Terrain &terrain,
                    const vec2i &offset);
};

} // namespace world

#endif // VKWORLD_DIAMONDSQUARETERRAIN_H
