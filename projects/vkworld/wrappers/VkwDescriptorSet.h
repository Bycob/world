#ifndef VKWORLD_DESCRIPTORSETVK_H
#define VKWORLD_DESCRIPTORSETVK_H

#include "vkworld/VkWorldConfig.h"

#include <memory>

#include <world/core/WorldTypes.h>

#include "Vulkan.h"
#include "IVkwBindable.h"
#include "VkwEnums.h"
#include "VkwDescriptorSetLayout.h"
#include "VkwSubBuffer.h"

namespace world {

class VkwDescriptorSetPrivate;

class VKWORLD_EXPORT VkwDescriptorSet {
public:
    VkwDescriptorSet(VkwDescriptorSetLayout &layout);

    void addDescriptor(u32 id, IVkwBindable &bindable);

    /** Allocate memory for struct, load in memory and add the
     * descriptor. */
    template <typename T> void addUniformStruct(u32 id, T &value);

    vk::DescriptorSet &handle();

private:
    std::shared_ptr<VkwDescriptorSetPrivate> _internal;

    void storeBuffer(VkwSubBuffer &buffer);
};

template <typename T>
void VkwDescriptorSet::addUniformStruct(u32 id, T &value) {
    auto &ctx = Vulkan::context();
    VkwSubBuffer buffer = ctx.allocate(
        sizeof(T), DescriptorType::UNIFORM_BUFFER, MemoryUsage::CPU_WRITES);
    buffer.setData(&value);
    storeBuffer(buffer);
    addDescriptor(id, buffer);
}

} // namespace world

#endif // VKWORLD_DESCRIPTORSETVK_H
