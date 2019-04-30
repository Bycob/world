#include "DescriptorSetVk.h"

#include <vulkan/vulkan.hpp>

#include "Vulkan.h"
#include "Vulkan_p.h"

namespace world {

class VkwDescriptorSetPrivate {
public:
    vk::DescriptorSet _descriptorSet;
};

VkwDescriptorSet::VkwDescriptorSet(VkwDescriptorSetLayout &layout)
        : _internal(std::make_shared<VkwDescriptorSetPrivate>()) {

    auto &vkctx = Vulkan::context().internal();

    // Allocate descriptor set in the pool
    vk::DescriptorSetAllocateInfo descriptorSetInfo(vkctx._descriptorPool, 1,
                                                    &layout.getLayout());
    _internal->_descriptorSet =
        vkctx._device.allocateDescriptorSets(descriptorSetInfo)[0];
}

void VkwDescriptorSet::addDescriptor(u32 id, DescriptorType usage,
                                     IVkwBindable &bindable) {
    bindable.registerTo(_internal->_descriptorSet,
                        Vulkan::context().internal().getDescriptorType(usage),
                        id);
}

vk::DescriptorSet &VkwDescriptorSet::handle() {
    return _internal->_descriptorSet;
}
} // namespace world
