#include "VkwRandomTexture.h"

#include "Vulkan.h"

namespace world {
VkwRandomTexture::VkwRandomTexture(u32 size)
        : VkwRandomTexture(size, std::random_device()()) {}

VkwRandomTexture::VkwRandomTexture(u32 size, u64 seed) : _rng(seed) {
    // Create random pixels
    const u32 count = size * size;
    std::uniform_int_distribution<u32> distrib(0, 0xFFFFFFFF);
    std::vector<u32> pixels;
    pixels.reserve(count);

    for (u32 i = 0; i < count; ++i) {
        pixels.push_back(distrib(_rng));
    }

    // Put pixels in vulkan image
    auto &ctx = Vulkan::context();

    _texture = std::make_unique<VkwImage>(
        VkwImageUsage::TEXTURE, vk::Format::eR8G8B8A8Unorm, size, size);
    _texture->setData(pixels.data(), count * sizeof(u32), 0);
}
} // namespace world
