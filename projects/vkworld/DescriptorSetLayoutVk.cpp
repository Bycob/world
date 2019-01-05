#include "DescriptorSetLayoutVk.h"

#include "Vulkan_p.h"

namespace world {

class DescriptorSetLayoutVkPrivate {
public:
    std::vector<vk::DescriptorSetLayoutBinding> _bindings;

    vk::DescriptorSetLayout _descriptorSetLayout;

    bool _initialized = false;


    void createDescriptorSetLayout() {
        auto &vkctx = Vulkan::context().internal();
        vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo(
            {}, static_cast<u32>(_bindings.size()), &_bindings[0]);
        _descriptorSetLayout = vkctx._device.createDescriptorSetLayout(
            descriptorSetLayoutCreateInfo);
    }
};

DescriptorSetLayoutVk::DescriptorSetLayoutVk()
        : _internal(std::make_shared<DescriptorSetLayoutVkPrivate>()) {}

void DescriptorSetLayoutVk::addBinding(DescriptorType usage, u32 binding) {
    vk::DescriptorType descriptorType =
        Vulkan::context().internal().getDescriptorType(usage);
    _internal->_bindings.emplace_back(binding, descriptorType, 1,
                                      vk::ShaderStageFlagBits::eCompute);
}

vk::DescriptorSetLayout &DescriptorSetLayoutVk::getLayout() {
    if (!_internal->_initialized) {
        _internal->createDescriptorSetLayout();
    }
    return _internal->_descriptorSetLayout;
}
} // namespace world
