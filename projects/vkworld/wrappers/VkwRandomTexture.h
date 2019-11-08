#ifndef VKWORLD_VKWRANDOMTEXTURE_H
#define VKWORLD_VKWRANDOMTEXTURE_H

#include "vkworld/VkWorldConfig.h"

#include <random>

#include "VkwImage.h"

namespace world {

/** Random texture generator. Ready for use textures. */
class VKWORLD_EXPORT VkwRandomTexture {
public:
    explicit VkwRandomTexture(u32 size = 256);

    VkwRandomTexture(u32 size, u64 seed);

    // TODO const
    VkwImage &get() { return *_texture; }

private:
    std::mt19937 _rng;
    std::unique_ptr<VkwImage> _texture;
};

} // namespace world

#endif // VKWORLD_VKWRANDOMTEXTURE_H
