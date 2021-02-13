#ifndef VKWORLD_VKW_MEMORY_HELPER_H
#define VKWORLD_VKW_MEMORY_HELPER_H

#include "vkworld/VkWorldConfig.h"

#include <world/assets/Image.h>
#include <world/terrain/Terrain.h>

#include "IVkwMemoryAccess.h"
#include "VkwImage.h"
#include "VkwMemoryCache.h"

namespace world {

struct VKWORLD_EXPORT VkwVertex {
    vec3f _position;
    vec3f _normal;
    vec2f _uv;

    VkwVertex(Vertex vert)
            : _position(vert.getPosition()), _normal(vert.getNormal()),
              _uv(vert.getTexture()) {}
};

class VKWORLD_EXPORT VkwMemoryHelper {
public:
    /** Copy GPU buffer into an image. The GPU buffer must contain floating
     * values between 0 and 1. This method assumes that the GPU buffer image
     * uses the same number of components as the destination image. If it is
     * not the case, you can use a variant of this method to specify the
     * number of components in the GPU buffer image. */
    static void GPUToImage(IVkwMemoryAccess &memory, Image &img);

    static void GPUToImageu(IVkwMemoryAccess &memory, Image &img);

    /** Copy GPU buffer into an image. The GPU buffer must contain floating
     * values between 0 and 1. `elemCount` indicates the number of components
     * of the image in the vulkan buffer. The destination image can have a
     * smaller number of components. */
    static void GPUToImage(IVkwMemoryAccess &memory, Image &img,
                           u32 elemCount); // TODO f

    static void GPUToImageu(IVkwMemoryAccess &memory, Image &img,
                            u32 elemCount);

    static Image GPUToImage(VkwImage &vkimg);

    static void imageToGPU(const Image &img, IVkwMemoryAccess &memory);

    static void GPUToTerrain(IVkwMemoryAccess &memory, Terrain &terrain);

    static void terrainToGPU(const Terrain &terrain, IVkwMemoryAccess &memory);

    static void GPUToMesh(IVkwMemoryAccess &verticesMemory,
                          IVkwMemoryAccess &indicesMemory, Mesh &mesh);

    static void GPUToVertices(IVkwMemoryAccess &memory, Mesh &mesh);

    static void GPUToIndices(IVkwMemoryAccess &memory, Mesh &mesh);

    /*
    static void toGPU(const Mesh &mesh, IVkwMemoryAccess &memory);
    */

    // No. This is not the good signature.
    static void copyBuffer(VkwSubBuffer &from, VkwSubBuffer &to);
};

} // namespace world

#endif // VKWORLD_VK_MEMORY_HELPER_H
