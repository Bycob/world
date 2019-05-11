#ifndef VKWORLD_VKW_MEMORY_HELPER_H
#define VKWORLD_VKW_MEMORY_HELPER_H

#include "VkWorldConfig.h"

#include <world/assets/Image.h>
#include <world/terrain/Terrain.h>

#include "IVkMemoryAccess.h"

namespace world {

class VKWORLD_EXPORT VkwMemoryHelper {
public:
    /** Copy GPU buffer into an image. The GPU buffer must contain floating
     * values between 0 and 1. The image type is taken into account. */
    static void GPUToImage(IVkwMemoryAccess &memory, Image &img);

    static void imageToGPU(const Image &img, IVkwMemoryAccess &memory);

    static void GPUToTerrain(IVkwMemoryAccess &memory, Terrain &terrain);

    static void terrainToGPU(const Terrain &terrain, IVkwMemoryAccess &memory);

    /*
    static void fromGPU(IVkMemoryAccess &memory, Mesh &mesh);

    static void toGPU(const Mesh &mesh, IVkwMemoryAccess &memory);
    */
};

}

#endif //VKWORLD_VK_MEMORY_HELPER_H
