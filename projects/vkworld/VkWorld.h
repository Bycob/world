#ifndef WORLD_VKWORLD_H
#define WORLD_VKWORLD_H

#include "VkWorldConfig.h"

#include "world/flat/FlatWorld.h"

namespace world {

class VKWORLD_EXPORT VkWorld {
public:
    static FlatWorld *createDemoFlatWorld();
};
} // namespace world

#endif // WORLD_VKWORLD_H
