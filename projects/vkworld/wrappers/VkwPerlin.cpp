#include "VkwPerlin.h"

namespace world {

VkwSubBuffer VkwPerlin::createPerlinHash() {
    std::mt19937 rng(std::random_device{}());

    std::vector<u32> random(256);
    std::iota(random.begin(), random.end(), 0);
    std::shuffle(random.begin(), random.end(), rng);
    random.insert(random.end(), random.begin(), random.end());

    auto &ctx = Vulkan::context();
    VkwSubBuffer buf =
        ctx.allocate(static_cast<u32>(random.size()) * sizeof(u32),
                     DescriptorType::STORAGE_BUFFER, MemoryUsage::CPU_WRITES);
    buf.setData(&random[0]);

    return buf;
}

void VkwPerlin::addPerlinHash(VkwDescriptorSet &dset, u32 id) {}
} // namespace world
