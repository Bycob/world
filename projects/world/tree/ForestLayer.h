#ifndef WORLD_FOREST_LAYER_H
#define WORLD_FOREST_LAYER_H

#include "world/core/WorldConfig.h"

#include <random>

#include "world/core/IChunkDecorator.h"
#include "world/flat/FlatWorld.h"
#include "world/assets/Image.h"
#include "Tree.h"

namespace world {

class WORLDAPI_EXPORT ForestLayer : public IChunkDecorator {
    WORLD_WRITE_SUBCLASS_METHOD
public:
    ForestLayer();

    void decorate(Chunk &chunk, const ExplorationContext &ctx) override;

    void write(WorldFile &wf) const override;

    void read(const WorldFile &wf) override;

private:
    std::mt19937 _rng;

    std::unique_ptr<Tree> _templateTree;

    Image _treeSprite;


    /// Maximum possible density of trees, in tree.km^-2
    double _maxDensity = 5000;
    // 20000 is better, but needs to be optimized both in memory and mesh
    // complexity

    double getDensityAtAltitude(double altitude);
};

} // namespace world

#endif // WORLD_FOREST_LAYER_H
