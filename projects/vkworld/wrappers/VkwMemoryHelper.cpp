#include "VkwMemoryHelper.h"

namespace world {

void VkwMemoryHelper::GPUToImage(IVkwMemoryAccess &memory, Image &img) {
    GPUToImage(memory, img, img.elemSize());
}

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

Image VkwMemoryHelper::GPUToImage(VkwImage &vkimg) {
    ImageType imType;

    switch (vkimg.format()) {
    case vk::Format::eR32G32B32A32Sfloat:
        imType = ImageType::RGBA;
        break;
    case vk::Format::eR32G32B32Sfloat:
        imType = ImageType::RGB;
        break;
    case vk::Format::eR32Sfloat:
        imType = ImageType::GREYSCALE;
        break;
    default:
        throw std::runtime_error("GPUToImage: Vk image format not supported (" +
                                 std::to_string(int(vkimg.format())) + ")");
    }
    Image img(vkimg.width(), vkimg.height(), imType);
    GPUToImage(vkimg, img);
    return img;
}

void VkwMemoryHelper::imageToGPU(const Image &img, IVkwMemoryAccess &memory) {}

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

} // namespace world
