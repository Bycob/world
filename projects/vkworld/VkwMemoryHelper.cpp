#include "VkwMemoryHelper.h"

namespace world {

void VkwMemoryHelper::GPUToImage(IVkwMemoryAccess &memory, Image &img) {
    const int w = img.width(), h = img.height(), e = img.elemSize();

    float *buffer = new float[img.size()];
    memory.getData(buffer, img.size() * sizeof(float), 0);

    for (u32 y = 0; y < h; ++y) {
        for (u32 x = 0; x < w; ++x) {
            u32 pos = (y * w + x) * e;
            img.setf(x, y, buffer + pos);
        }
    }
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

} // namespace world
