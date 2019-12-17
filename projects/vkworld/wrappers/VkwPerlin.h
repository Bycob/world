#ifndef WORLD_VKWPERLIN_H
#define WORLD_VKWPERLIN_H

#include "vkworld/VkWorldConfig.h"

#include "world/math/Perlin.h"
#include "world/core/WorldTypes.h"

#include "vkworld/wrappers/VkwDescriptorSet.h"

namespace world {

struct VKWORLD_EXPORT VkwPerlinParameters {
    u32 octaves;
    u32 octaveRef;
    s32 offsetX;
    s32 offsetY;
    s32 offsetZ = 0;
    float frequency;
    float persistence = 1.2f;

    VkwPerlinParameters() = default;

    VkwPerlinParameters(const PerlinInfo &info)
            : octaves(info.octaves), octaveRef(info.reference),
              offsetX(info.offsetX), offsetY(info.offsetY),
              frequency(info.frequency), persistence(info.persistence) {}
};

class VKWORLD_EXPORT VkwPerlin {
public:
    static VkwSubBuffer createPerlinHash();

    static void addPerlinHash(VkwDescriptorSet &dset, u32 id = 256);
};
} // namespace world

#endif // WORLD_VKWPERLIN_H
