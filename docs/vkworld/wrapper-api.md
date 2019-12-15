# VkWorld Vulkan Wrapper API

## Automatic resource management

For the sake of simplicity, all wrapped Vulkan resources are kept in shared_ptr.

Some of the wrappers keep track of the resources they need, so it is not required that the host keeps a reference.
For example, a VkwDescriptorSet attached to a worker can be dropped.

Here is a recap of the resources held by different wrappers:

| Wrapper ... | ... keeps track of ... |
| --- | --- |
| VkwDescriptorSet | VkwSubBuffer, VkwImage, VkwDescriptorSetLayout |
| VkwRenderPass | VkwImage |
| Vkw*Pipeline | VkwDescriptorSetLayout, VkwRenderPass |
| Vkw*Worker | Vkw*Pipelines, VkwDescriptorSet |