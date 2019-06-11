#include "DescriptorSetLayoutVk.h"

#include "Vulkan.h"

namespace world {

class VkwDescriptorSetLayoutPrivate {
public:
    std::vector<vk::DescriptorSetLayoutBinding> _bindings;

    vk::DescriptorSetLayout _descriptorSetLayout;

    bool _initialized = false;


    void createDescriptorSetLayout() {
        auto &ctx = Vulkan::context();
        vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo(
            {}, static_cast<u32>(_bindings.size()), &_bindings[0]);
        _descriptorSetLayout = ctx._device.createDescriptorSetLayout(
            descriptorSetLayoutCreateInfo);
    }
};

VkwDescriptorSetLayout::VkwDescriptorSetLayout()
        : _internal(std::make_shared<VkwDescriptorSetLayoutPrivate>()) {}

VkwDescriptorSetLayout::VkwDescriptorSetLayout(
    std::initializer_list<u32> uniformIds,
    std::initializer_list<u32> storageIds)
        : VkwDescriptorSetLayout() {
    for (u32 id : uniformIds) {
        addBinding(DescriptorType::UNIFORM_BUFFER, id);
    }
    for (u32 id : storageIds) {
        addBinding(DescriptorType::STORAGE_BUFFER, id);
    }
}

void VkwDescriptorSetLayout::addBinding(DescriptorType usage, u32 binding) {
    vk::DescriptorType descriptorType =
        Vulkan::context().getDescriptorType(usage);
    _internal->_bindings.emplace_back(binding, descriptorType, 1,
                                      vk::ShaderStageFlagBits::eCompute);
}

vk::DescriptorType VkwDescriptorSetLayout::getBindingType(u32 id) const {
    for (auto &binding : _internal->_bindings) {
        if (binding.binding == id) {
            return binding.descriptorType;
        }
    }
    throw std::invalid_argument("No binding for this id: " +
                                std::to_string(id));
}

vk::DescriptorSetLayout &VkwDescriptorSetLayout::getLayout() {
    if (!_internal->_initialized) {
        _internal->createDescriptorSetLayout();
    }
    return _internal->_descriptorSetLayout;
}
} // namespace world
