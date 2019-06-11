#include "VkwDescriptorSet.h"

#include <vulkan/vulkan.hpp>

#include "Vulkan.h"

namespace world {

class VkwDescriptorSetPrivate {
public:
    VkwDescriptorSetLayout _layout;
    vk::DescriptorSet _descriptorSet;
};

VkwDescriptorSet::VkwDescriptorSet(VkwDescriptorSetLayout &layout)
        : _internal(std::make_shared<VkwDescriptorSetPrivate>()) {

    auto &ctx = Vulkan::context();

    // Allocate descriptor set in the pool
    vk::DescriptorSetAllocateInfo descriptorSetInfo(ctx._descriptorPool, 1,
                                                    &layout.getLayout());
    _internal->_layout = layout;
    _internal->_descriptorSet =
        ctx._device.allocateDescriptorSets(descriptorSetInfo)[0];
}

void VkwDescriptorSet::addDescriptor(u32 id, IVkwBindable &bindable) {
    bindable.registerTo(_internal->_descriptorSet,
                        _internal->_layout.getBindingType(id), id);
}

vk::DescriptorSet &VkwDescriptorSet::handle() {
    return _internal->_descriptorSet;
}
} // namespace world
