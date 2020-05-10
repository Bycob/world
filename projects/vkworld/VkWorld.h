#ifndef WORLD_VKWORLD_H
#define WORLD_VKWORLD_H

#include "VkWorldConfig.h"

#include "world/flat/FlatWorld.h"

namespace world {

class VKWORLD_EXPORT VkWorld {
public:
    /** Force to load the Vkworld library even if no symbols is used.
     * Some applications may require this trick, for example if they
     * must be able to read a serialized vkworld that contains modules
     * from VkWorld. */
    static void loadLibrary();

    static FlatWorld *createDemoFlatWorld();
};
} // namespace world

#endif // WORLD_VKWORLD_H
