#include "VkwMemoryHelper.h"

#include "Vulkan.h"

namespace world {

void VkwMemoryHelper::GPUToImage(IVkwMemoryAccess &memory, Image &img) {
    GPUToImage(memory, img, img.elemSize());
}

void VkwMemoryHelper::GPUToImageu(IVkwMemoryAccess &memory, Image &img) {
    GPUToImageu(memory, img, img.elemSize());
}

// TODO change int to size_t to avoid overflow
void VkwMemoryHelper::GPUToImage(IVkwMemoryAccess &memory, Image &img, u32 e) {
    const int w = img.width(), h = img.height();
    const int size = w * h * e;

    float *buffer = new float[size];
    memory.getData(buffer, size * sizeof(float), 0);

    for (u32 y = 0; y < h; ++y) {
        for (u32 x = 0; x < w; ++x) {
            u32 pos = (y * w + x) * e;

            img.setf(x, y, buffer + pos);
        }
    }
}

void VkwMemoryHelper::GPUToImageu(IVkwMemoryAccess &memory, Image &img, u32 e) {
    const int w = img.width(), h = img.height();
    const int size = w * h * e;

    u8 *buffer = new u8[size];
    memory.getData(buffer, size * sizeof(u8), 0);

    for (u32 y = 0; y < h; ++y) {
        for (u32 x = 0; x < w; ++x) {
            u32 pos = (y * w + x) * e;

            img.set(x, y, buffer + pos);
        }
    }
}

Image VkwMemoryHelper::GPUToImage(VkwImage &vkimg) {
    ImageType imType;
    bool isFloat = false;

    switch (vkimg.format()) {
    case vk::Format::eR32G32B32A32Sfloat:
        isFloat = true;
    case vk::Format::eR8G8B8A8Unorm:
        imType = ImageType::RGBA;
        break;
    case vk::Format::eR32G32B32Sfloat:
        isFloat = true;
    case vk::Format::eR8G8B8Unorm:
        imType = ImageType::RGB;
        break;
    case vk::Format::eR32Sfloat:
        isFloat = true;
    case vk::Format::eR8Unorm:
        imType = ImageType::GREYSCALE;
        break;
    default:
        throw std::runtime_error("GPUToImage: Vk image format not supported (" +
                                 std::to_string(int(vkimg.format())) + ")");
    }
    Image img(vkimg.width(), vkimg.height(), imType);
    if (isFloat) {
        GPUToImage(vkimg, img);
    } else {
        GPUToImageu(vkimg, img);
    }
    return img;
}

void VkwMemoryHelper::imageToGPU(const Image &img, IVkwMemoryAccess &memory) {
    // TODO VkwMemoryHelper::imageToGPU
}

void VkwMemoryHelper::GPUToTerrain(IVkwMemoryAccess &memory, Terrain &terrain) {

}

void VkwMemoryHelper::terrainToGPU(const Terrain &terrain,
                                   IVkwMemoryAccess &memory) {
    int res = terrain.getResolution();
    float *buf = new float[res * res];

    for (int y = 0; y < res; ++y) {
        for (int x = 0; x < res; ++x) {
            buf[y * res + x] = float(terrain(x, y));
        }
    }
    memory.setData(buf, res * res * sizeof(float), 0);

    delete[] buf;
}

void VkwMemoryHelper::GPUToMesh(IVkwMemoryAccess &verticesMemory,
                                IVkwMemoryAccess &indicesMemory, Mesh &mesh) {
    GPUToVertices(verticesMemory, mesh);
    GPUToIndices(indicesMemory, mesh);
}

void VkwMemoryHelper::GPUToVertices(IVkwMemoryAccess &memory, Mesh &mesh) {}

void VkwMemoryHelper::GPUToIndices(IVkwMemoryAccess &memory, Mesh &mesh) {}

void VkwMemoryHelper::copyBuffer(VkwSubBuffer &from, VkwSubBuffer &to) {
    auto &ctx = Vulkan::context();

    vk::CommandBufferAllocateInfo commandBufInfo(
        ctx._graphicsCommandPool, vk::CommandBufferLevel::ePrimary, 1);
    auto commandBuf = ctx._device.allocateCommandBuffers(commandBufInfo).at(0);
    commandBuf.begin(vk::CommandBufferBeginInfo(
        vk::CommandBufferUsageFlagBits::eOneTimeSubmit));

    /*insertBarrier(commandBuf, {}, vk::AccessFlagBits ::eTransferWrite,
        vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral,
        vk::PipelineStageFlagBits::eTopOfPipe,
        vk::PipelineStageFlagBits::eTransfer);*/

    vk::BufferCopy bufCopy{from.getOffset(), to.getOffset(), from.getSize()};
    commandBuf.copyBuffer(from.handle(), to.handle(), bufCopy);

    /*insertBarrier(commandBuf, vk::AccessFlagBits ::eTransferWrite,
        vk::AccessFlagBits ::eShaderRead, vk::ImageLayout::eGeneral,
        vk::ImageLayout::eGeneral,
        vk::PipelineStageFlagBits::eTransfer,
        vk::PipelineStageFlagBits::eFragmentShader);*/

    commandBuf.end();

    auto fence = ctx._device.createFence(vk::FenceCreateInfo());
    vk::SubmitInfo submitInfo(0, nullptr, nullptr, 1, &commandBuf);
    ctx.queue(vk::QueueFlagBits::eGraphics).submit(submitInfo, fence);

    ctx._device.waitForFences(fence, true, 1000000000000);

    // Destroy resources
    ctx._device.freeCommandBuffers(ctx._graphicsCommandPool, commandBuf);
    ctx._device.destroyFence(fence);
}


} // namespace world
