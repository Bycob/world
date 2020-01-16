#ifndef VKWORLD_VKWSTATS_H
#define VKWORLD_VKWSTATS_H

#include <vkworld/VkWorldConfig.h>

#include <iostream>

#include <world/core/WorldTypes.h>

namespace world {

/** This struct tracks some information about allocated resources and
 * general context. It can be used for debug purpose. */
struct VkwStats {
    u32 _descriptorSetCount = 0;
};

inline std::ostream &operator<<(std::ostream &stream, const VkwStats &stats) {
    stream << "Current descriptor set count: " << stats._descriptorSetCount
           << std::endl;
    return stream;
}

} // namespace world

#endif // VKWORLD_VKWSTATS_H
