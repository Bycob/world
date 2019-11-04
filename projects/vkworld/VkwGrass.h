#ifndef VKWORLD_GRASS_H
#define VKWORLD_GRASS_H

#include "VkWorldConfig.h"

#include <world/core/WorldNode.h>

namespace world {

class VkwGrassPrivate;

class VKWORLD_EXPORT VkwGrass : public WorldNode {
public:
    VkwGrass();
    ~VkwGrass() override;

protected:
    void collectSelf(ICollector &collector,
                     const IResolutionModel &resolutionModel,
                     const ExplorationContext &ctx) override;

private:
    VkwGrassPrivate *_internal;
    bool _isInitialized = false;


    void setup();
};

} // namespace world

#endif // VKWORLD_GRASS_H
