#ifndef VKWORLD_GRASS_H
#define VKWORLD_GRASS_H

#include "VkWorldConfig.h"

#include <world/core/WorldNode.h>
#include <world/core/IInstanceGenerator.h>

namespace world {

class VkwGrassPrivate;

class VKWORLD_EXPORT VkwGrass : public WorldNode, public IInstanceGenerator {
public:
    VkwGrass();
    ~VkwGrass() override;

    std::vector<Template> collectTemplates(ICollector &collector,
                                           const ExplorationContext &ctx);

    HabitatFeatures randomize();

protected:
    void collectSelf(ICollector &collector,
                     const IResolutionModel &resolutionModel,
                     const ExplorationContext &ctx) override;

private:
    VkwGrassPrivate *_internal;
    bool _isInitialized = false;

    std::mt19937 _rng;

    u32 _count = 20;
    /** Percent of height */
    double _bend = 0.3;
    /** Percent of height */
    double _fold = 0.5;
    /** Size of the final mesh, in meters. */
    double _size = 0.3;
    /** height, in meters */
    double _height = 0.3;
    /** width, in meters */
    double _width = 0.02;

    void setup();
};

} // namespace world

#endif // VKWORLD_GRASS_H
