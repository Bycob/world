#include <iostream>
#include <stdexcept>
#include <random>
#include <algorithm>
#include <iterator>

#include <world/core.h>
#include <world/terrain.h>

#include <vkworld/wrappers/Vulkan.h>
#include <vkworld/wrappers/VkwRandomTexture.h>
#include <vkworld/MultilayerGroundTextureOld.h>
#include <vkworld/VkwMultilayerGroundTexture.h>
#include <vkworld/wrappers/VkwTextureGenerator.h>
#include <vkworld/wrappers/VkwMemoryHelper.h>
#include <vkworld/VkwGrass.h>
#include <vkworld/VkwLeafTexture.h>

using namespace world;

void testCompute();
void testRandomImage();
void testMultilayerTerrainTexture(int argc, char **argv);
void testTextureGenerator();
void testVkwGrass();
void testLeaves();
void workingExample();

int main(int argc, char **argv) {
    world::createDirectories("assets/vulkan/");

    // testCompute();
    // testMultilayerTerrainTexture(argc, argv);
    testTextureGenerator();
    // testVkwGrass();
    // testLeaves();
    // std::cout << "====" << std::endl;
    // testRandomImage();
    // workingExample();
}

void testCompute() {
    // This method contains several notes to improve the API later on
    int size = 1024;
    int groupSize = 32;
    int dispatchSize = size / groupSize;
    
    std::cout << "Set up..." << std::endl;

    VkwComputeWorker worker;

    VkwDescriptorSetLayout layout({0, 1}, {3, 256});

    // Buffers
    auto &ctx = Vulkan::context();

    // -- 0
    vec3u dimsVec(size, size, 1);
    auto dims = ctx.allocate(sizeof(vec3u), DescriptorType::UNIFORM_BUFFER, MemoryUsage::CPU_WRITES);
    dims.setData(&dimsVec); // TODO add setData(const T &struct)

    // -- 1
    struct {
        u32 octaves = 5;
        u32 octaveRef = 0;
        s32 offsetX = 0;
        s32 offsetY = 0;
        s32 offsetZ = 0;
        float frequency = 4.0f;
        float persistence = 0.5f;
    } noiseStruct;

    auto noiseParams = ctx.allocate(sizeof(noiseStruct), DescriptorType::UNIFORM_BUFFER, MemoryUsage::CPU_WRITES);
    noiseParams.setData(&noiseStruct);

    // -- 256
    Perlin perlin;
    std::vector<u32> randVec;

    for (u8 u : perlin.getHash()) {
        randVec.push_back(u);
    }

    auto random = ctx.allocate(randVec.size() * sizeof(u32), DescriptorType::STORAGE_BUFFER, MemoryUsage::CPU_WRITES);
    random.setData(&randVec[0]);

    // -- 3
    auto output = ctx.allocate(size * size * sizeof(float), DescriptorType::STORAGE_BUFFER, MemoryUsage::CPU_READS);


    VkwDescriptorSet dset(layout);
    dset.addDescriptor(0, dims);
    dset.addDescriptor(1, noiseParams);
    dset.addDescriptor(3, output);
    dset.addDescriptor(256, random);

    VkwComputePipeline pipeline(layout, "noise-perlin.comp");

    worker.bindCommand(pipeline, dset);
    worker.dispatchCommand(dispatchSize, dispatchSize, 1);
    worker.endCommandRecording(); // endCommand in run ?

    std::cout << "Running..." << std::endl;
    worker.run();
    worker.waitForCompletion();

    Image img(size, size, ImageType::GREYSCALE);
    VkwMemoryHelper::GPUToImage(output, img);

    std::cout << "Completed! Writing to assets/vulkan/compute.png" << std::endl;
    img.write("assets/vulkan/compute.png");
}

void testRandomImage() {
    // Write a random image, then read it from GPU and write it on disk
    VkwRandomTexture randTex(256);

    world::createDirectories("assets/vulkan/");
    VkwMemoryHelper::GPUToImage(randTex.get()).write("assets/vulkan/randTex.png");
    std::cout << "wrote assets/vulkan/randTex.png" << std::endl;
}

// TODO
// - Profiling
// - Clean world3D to use vkworld only if it is available
// - Change array of textures to texture arrays
// - Transition images correctly, use the right layout for the right usage
// - Fix vulkan warnings
// - Use the right compile definition with cmake. Documentation about it.
// - Implement what it takes to deallocate resources and not saturate the RAM
// - Address potential performance issues
void testMultilayerTerrainTexture(int argc, char **argv) {
    Terrain terrain(128);
    terrain.setBounds(-500, -500, 0, 500, 500, 400);
    terrain.setTexture(Image(128 * 16, 128 * 16, ImageType::RGBA));

    PerlinTerrainGenerator terrainGen(5, 4, 0.4);
    terrainGen.processTerrain(terrain);

    VkwMultilayerGroundTexture textureGen;
    textureGen.addDefaultLayers();
    textureGen.processTerrain(terrain);
    terrain.setBounds(-0.5, -0.5, 0, 0.5, 0.5, 0.4);

    Mesh *mesh = terrain.createMesh();
    SceneNode object("mesh1");
    object.setMaterialID("multilayer");

    Material mat("multilayer");
    mat.setMapKd("multilayer_texture.png");

    Scene scene;
    scene.addNode(object);
    scene.addMesh("mesh1", *mesh);
    scene.addMaterial("multilayer", mat);
    scene.addTexture("multilayer_texture.png", terrain.getTexture());


    world::createDirectories("assets/vulkan/multilayer/");
    ObjLoader obj;
    obj.write(scene, "assets/vulkan/multilayer/multilayer.obj");

    delete mesh;
}

/*
Debug vulkan:
- Skipped validation layers (I don't think the pb is here)
- No descriptor pool in theirs -> sépassa
- No depth buffer in mine -> sépassa
- VkImage is both used as "Transfer Src" & "Sampled"
- VkRenderPass: final layout is img::general instead of transfer
- vkDeviceWaitIdle
- No depth & stencil buffer
- subpass dependency -> might be the thing

- no one time submit
- no bind descriptor set layout
- pipeline cache
- no line width rasterization
- blend enable = false & pas les trucs par défaut
- dynamic set viewport
- begin render pass VK subpass content inline
- verifier queue family index (chez moi = 0)
- (peut-être) vérifier image get data
*/

void testTextureGenerator() {
#define GEN_SHADER_NAME "test-texture.frag"

    const u32 size = 1024;
    VkwTextureGenerator generator(size, size, GEN_SHADER_NAME);

#ifdef GEN_GRASS
    // Texture size
    struct {
        float offsetX = 0, offsetY = 0;
        float sizeX = 0.5, sizeY = 0.5;
    } textureStruct;
    generator.addParameter(0, DescriptorType::UNIFORM_BUFFER,
                           MemoryUsage::CPU_WRITES, sizeof(textureStruct),
                           &textureStruct);
#endif

#ifdef GEN_PERLIN
    // Random
    std::mt19937 _rng(42);
    std::vector<u32> random(256);
    std::iota(random.begin(), random.end(), 0);
    std::shuffle(random.begin(), random.end(), _rng);
    random.insert(random.end(), random.begin(), random.end());

    generator.addParameter(256, DescriptorType::STORAGE_BUFFER,
                           MemoryUsage::CPU_WRITES, random.size() * sizeof(u32),
                           &random[0]);
#endif

#ifdef GEN_RAND_TEX
    // Random texture
    VkwRandomTexture randomTex;
    generator.addImageParameter(0, randomTex.get());
#endif

    std::cout << "writing to "
        << "assets/vulkan/test_generator.png" << std::endl;
    generator.generateTexture().write("assets/vulkan/test_generator.png");
}


void testVkwGrass() {
    VkwGrass grass;

    Collector collector(CollectorPresets::SCENE);
    grass.collectAll(collector, 10);

    Image img = (*collector.getStorageChannel<Image>().begin())._value;
    std::cout << "wrote assets/vulkan/vkwgrass.png" << std::endl;
    img.write("assets/vulkan/vkwgrass.png");
}

void testLeaves() {
    VkwLeafTexture leaves;
    std::cout << "writing to "
              << "assets/vulkan/vkwleaf.png" << std::endl;
    leaves.generateLeafTexture().write("assets/vulkan/vkwleaf.png");
}


// ====================================
// working example from sasha willem

#include "VulkanInitializers.hpp"

#define VK_CHECK_RESULT(f)																				\
{																										\
	VkResult res = (f);																					\
	if (res != VK_SUCCESS)																				\
	{																									\
		std::cout << "Fatal : VkResult is \"" << res << "\" in " << __FILE__ << " at line " << __LINE__ << "\n"; \
		assert(res == VK_SUCCESS);																		\
	}																									\
}

void insertImageMemoryBarrier(
    VkCommandBuffer cmdbuffer,
    VkImage image,
    VkAccessFlags srcAccessMask,
    VkAccessFlags dstAccessMask,
    VkImageLayout oldImageLayout,
    VkImageLayout newImageLayout,
    VkPipelineStageFlags srcStageMask,
    VkPipelineStageFlags dstStageMask,
    VkImageSubresourceRange subresourceRange)
{
    VkImageMemoryBarrier imageMemoryBarrier = vks::initializers::imageMemoryBarrier();
    imageMemoryBarrier.srcAccessMask = srcAccessMask;
    imageMemoryBarrier.dstAccessMask = dstAccessMask;
    imageMemoryBarrier.oldLayout = oldImageLayout;
    imageMemoryBarrier.newLayout = newImageLayout;
    imageMemoryBarrier.image = image;
    imageMemoryBarrier.subresourceRange = subresourceRange;

    vkCmdPipelineBarrier(
        cmdbuffer,
        srcStageMask,
        dstStageMask,
        0,
        0, nullptr,
        0, nullptr,
        1, &imageMemoryBarrier);
}

/*
Submit command buffer to a queue and wait for fence until queue operations have been finished
*/
void submitWork(VkCommandBuffer cmdBuffer, VkQueue queue)
{
    auto &ctx = Vulkan::context();
    VkDevice device = ctx._device;

    VkSubmitInfo submitInfo = vks::initializers::submitInfo();
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmdBuffer;
    VkFenceCreateInfo fenceInfo = vks::initializers::fenceCreateInfo();
    VkFence fence;
    VK_CHECK_RESULT(vkCreateFence(device, &fenceInfo, nullptr, &fence));
    VK_CHECK_RESULT(vkQueueSubmit(queue, 1, &submitInfo, fence));
    VK_CHECK_RESULT(vkWaitForFences(device, 1, &fence, VK_TRUE, UINT64_MAX));
    vkDestroyFence(device, fence, nullptr);
}

VkResult createBuffer(VkBufferUsageFlags usageFlags, vk::MemoryPropertyFlags memoryPropertyFlags, VkBuffer *buffer, VkDeviceMemory *memory, VkDeviceSize size, void *data = nullptr)
{
    auto &ctx = Vulkan::context();
    VkDevice device = ctx._device;

    // Create the buffer handle
    VkBufferCreateInfo bufferCreateInfo = vks::initializers::bufferCreateInfo(usageFlags, size);
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    VK_CHECK_RESULT(vkCreateBuffer(device, &bufferCreateInfo, nullptr, buffer));

    // Create the memory backing up the buffer handle
    VkMemoryRequirements memReqs;
    VkMemoryAllocateInfo memAlloc = vks::initializers::memoryAllocateInfo();
    vkGetBufferMemoryRequirements(device, *buffer, &memReqs);
    memAlloc.allocationSize = memReqs.size;
    memAlloc.memoryTypeIndex = ctx.findMemoryType(0, memoryPropertyFlags, {}, memReqs.memoryTypeBits);
    VK_CHECK_RESULT(vkAllocateMemory(device, &memAlloc, nullptr, memory));

    if (data != nullptr) {
        void *mapped;
        VK_CHECK_RESULT(vkMapMemory(device, *memory, 0, size, 0, &mapped));
        memcpy(mapped, data, size);
        vkUnmapMemory(device, *memory);
    }

    VK_CHECK_RESULT(vkBindBufferMemory(device, *buffer, *memory, 0));

    return VK_SUCCESS;
}

void workingExample() {
    auto &ctx = Vulkan::context();
    // ctx variables
    VkDevice device = ctx._device;
    VkQueue queue = ctx.queue(vk::QueueFlagBits::eGraphics);
    VkCommandPool commandPool = ctx._graphicsCommandPool;

    // Variables
    VkDescriptorSetLayout descriptorSetLayout;
    VkPipelineLayout pipelineLayout;
    std::vector<VkShaderModule> shaderModules;
    VkBuffer vertexBuffer, indexBuffer;
    VkDeviceMemory vertexMemory, indexMemory;
    VkFramebuffer framebuffer;
    VkRenderPass renderPass;
    VkPipeline pipeline;
    VkPipelineCache pipelineCache;

    VkCommandBuffer commandBuffer;
    VkCommandBufferAllocateInfo cmdBufAllocateInfo =
        vks::initializers::commandBufferAllocateInfo(commandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1);
    VK_CHECK_RESULT(vkAllocateCommandBuffers(device, &cmdBufAllocateInfo, &commandBuffer));

    VkCommandBufferBeginInfo cmdBufInfo =
        vks::initializers::commandBufferBeginInfo();

    VK_CHECK_RESULT(vkBeginCommandBuffer(commandBuffer, &cmdBufInfo));

    /*
    Prepare vertex and index buffers
    */
    struct Vertex {
        float position[3];
        float color[3];
    };
    {
        std::vector<Vertex> vertices = {
            { {  1.0f,  1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f } },
        { { -1.0f,  1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f } },
        { {  0.0f, -1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } }
        };
        std::vector<uint32_t> indices = { 0, 1, 2 };

        const VkDeviceSize vertexBufferSize = vertices.size() * sizeof(Vertex);
        const VkDeviceSize indexBufferSize = indices.size() * sizeof(uint32_t);

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingMemory;

        // Command buffer for copy commands (reused)
        VkCommandBufferAllocateInfo cmdBufAllocateInfo = vks::initializers::commandBufferAllocateInfo(commandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1);
        VkCommandBuffer copyCmd;
        VK_CHECK_RESULT(vkAllocateCommandBuffers(device, &cmdBufAllocateInfo, &copyCmd));
        VkCommandBufferBeginInfo cmdBufInfo = vks::initializers::commandBufferBeginInfo();

        // Copy input data to VRAM using a staging buffer
        {
            // Vertices
            createBuffer(
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
                &stagingBuffer,
                &stagingMemory,
                vertexBufferSize,
                vertices.data());

            createBuffer(
                VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                vk::MemoryPropertyFlagBits::eDeviceLocal,
                &vertexBuffer,
                &vertexMemory,
                vertexBufferSize);

            VK_CHECK_RESULT(vkBeginCommandBuffer(copyCmd, &cmdBufInfo));
            VkBufferCopy copyRegion = {};
            copyRegion.size = vertexBufferSize;
            vkCmdCopyBuffer(copyCmd, stagingBuffer, vertexBuffer, 1, &copyRegion);
            VK_CHECK_RESULT(vkEndCommandBuffer(copyCmd));

            submitWork(copyCmd, queue);

            vkDestroyBuffer(device, stagingBuffer, nullptr);
            vkFreeMemory(device, stagingMemory, nullptr);

            // Indices
            createBuffer(
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
                &stagingBuffer,
                &stagingMemory,
                indexBufferSize,
                indices.data());

            createBuffer(
                VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                vk::MemoryPropertyFlagBits::eDeviceLocal,
                &indexBuffer,
                &indexMemory,
                indexBufferSize);

            VK_CHECK_RESULT(vkBeginCommandBuffer(copyCmd, &cmdBufInfo));
            copyRegion.size = indexBufferSize;
            vkCmdCopyBuffer(copyCmd, stagingBuffer, indexBuffer, 1, &copyRegion);
            VK_CHECK_RESULT(vkEndCommandBuffer(copyCmd));

            submitWork(copyCmd, queue);

            vkDestroyBuffer(device, stagingBuffer, nullptr);
            vkFreeMemory(device, stagingMemory, nullptr);
        }
    }

    /*
    Create framebuffer attachments
    */
    int width = 1024;
    int height = 1024;
    vk::Format colorFormat = vk::Format::eR32G32B32A32Sfloat;
    VkFormat colorFormatC = VK_FORMAT_R32G32B32A32_SFLOAT;
    VkwImage image(width, height, VkwImageUsage::OFFSCREEN_RENDER, colorFormat);

    /*
    Create renderpass
    */
    {
        std::array<VkAttachmentDescription, 1> attchmentDescriptions = {};
        // Color attachment
        attchmentDescriptions[0].format = colorFormatC;
        attchmentDescriptions[0].samples = VK_SAMPLE_COUNT_1_BIT;
        attchmentDescriptions[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attchmentDescriptions[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attchmentDescriptions[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attchmentDescriptions[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attchmentDescriptions[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attchmentDescriptions[0].finalLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

        VkAttachmentReference colorReference = { 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };

        VkSubpassDescription subpassDescription = {};
        subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpassDescription.colorAttachmentCount = 1;
        subpassDescription.pColorAttachments = &colorReference;
        subpassDescription.pDepthStencilAttachment = nullptr;

        // Use subpass dependencies for layout transitions
        std::array<VkSubpassDependency, 2> dependencies;

        dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
        dependencies[0].dstSubpass = 0;
        dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

        dependencies[1].srcSubpass = 0;
        dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
        dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

        // Create the actual renderpass
        VkRenderPassCreateInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = static_cast<uint32_t>(attchmentDescriptions.size());
        renderPassInfo.pAttachments = attchmentDescriptions.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpassDescription;
        renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
        renderPassInfo.pDependencies = dependencies.data();
        VK_CHECK_RESULT(vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass));

        VkImageView attachments[1];
        attachments[0] = image.getImageView();

        VkFramebufferCreateInfo framebufferCreateInfo = vks::initializers::framebufferCreateInfo();
        framebufferCreateInfo.renderPass = renderPass;
        framebufferCreateInfo.attachmentCount = 1;
        framebufferCreateInfo.pAttachments = attachments;
        framebufferCreateInfo.width = width;
        framebufferCreateInfo.height = height;
        framebufferCreateInfo.layers = 1;
        VK_CHECK_RESULT(vkCreateFramebuffer(device, &framebufferCreateInfo, nullptr, &framebuffer));
    }

    /*
    Prepare graphics pipeline
    */
    {
        std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings = {};
        VkDescriptorSetLayoutCreateInfo descriptorLayout =
            vks::initializers::descriptorSetLayoutCreateInfo(setLayoutBindings);
        VK_CHECK_RESULT(vkCreateDescriptorSetLayout(device, &descriptorLayout, nullptr, &descriptorSetLayout));

        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo =
            vks::initializers::pipelineLayoutCreateInfo(nullptr, 0);

        VK_CHECK_RESULT(vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout));

        VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {};
        pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
        VK_CHECK_RESULT(vkCreatePipelineCache(device, &pipelineCacheCreateInfo, nullptr, &pipelineCache));

        // Create pipeline
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyState =
            vks::initializers::pipelineInputAssemblyStateCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 0, VK_FALSE);

        VkPipelineRasterizationStateCreateInfo rasterizationState =
            vks::initializers::pipelineRasterizationStateCreateInfo(VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_CLOCKWISE);

        VkPipelineColorBlendAttachmentState blendAttachmentState =
            vks::initializers::pipelineColorBlendAttachmentState(0xf, VK_FALSE);

        VkPipelineColorBlendStateCreateInfo colorBlendState =
            vks::initializers::pipelineColorBlendStateCreateInfo(1, &blendAttachmentState);


        VkPipelineViewportStateCreateInfo viewportState =
            vks::initializers::pipelineViewportStateCreateInfo(1, 1);

        VkPipelineMultisampleStateCreateInfo multisampleState =
            vks::initializers::pipelineMultisampleStateCreateInfo(VK_SAMPLE_COUNT_1_BIT);

        std::vector<VkDynamicState> dynamicStateEnables = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };
        VkPipelineDynamicStateCreateInfo dynamicState =
            vks::initializers::pipelineDynamicStateCreateInfo(dynamicStateEnables);

        VkGraphicsPipelineCreateInfo pipelineCreateInfo =
            vks::initializers::pipelineCreateInfo(pipelineLayout, renderPass);

        std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages{};

        pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
        pipelineCreateInfo.pRasterizationState = &rasterizationState;
        pipelineCreateInfo.pColorBlendState = &colorBlendState;
        pipelineCreateInfo.pMultisampleState = &multisampleState;
        pipelineCreateInfo.pViewportState = &viewportState;
        pipelineCreateInfo.pDepthStencilState = nullptr;
        pipelineCreateInfo.pDynamicState = &dynamicState;
        pipelineCreateInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
        pipelineCreateInfo.pStages = shaderStages.data();

        // Vertex bindings an attributes
        // Binding description
        std::vector<VkVertexInputBindingDescription> vertexInputBindings = {
        };

        // Attribute descriptions
        std::vector<VkVertexInputAttributeDescription> vertexInputAttributes = {
        };

        VkPipelineVertexInputStateCreateInfo vertexInputState = vks::initializers::pipelineVertexInputStateCreateInfo();
        vertexInputState.vertexBindingDescriptionCount = static_cast<uint32_t>(vertexInputBindings.size());
        vertexInputState.pVertexBindingDescriptions = vertexInputBindings.data();
        vertexInputState.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexInputAttributes.size());
        vertexInputState.pVertexAttributeDescriptions = vertexInputAttributes.data();

        pipelineCreateInfo.pVertexInputState = &vertexInputState;

        // const std::string shadersPath = "shaders/";
        const std::string shadersPath = "../bin/Release/shaders/";

        shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        shaderStages[0].pName = "main";
        shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        shaderStages[1].pName = "main";
        shaderStages[0].module = ctx.createShader(ctx.readFile(shadersPath + "generic-texture.vert.spv"));
        shaderStages[1].module = ctx.createShader(ctx.readFile(shadersPath + "test-texture.frag.spv"));
        shaderModules = { shaderStages[0].module, shaderStages[1].module };
        VK_CHECK_RESULT(vkCreateGraphicsPipelines(device, pipelineCache, 1, &pipelineCreateInfo, nullptr, &pipeline));
    }

    /*
    Command buffer creation
    */
    {
        VkClearValue clearValues[2];
        clearValues[0].color = { { 0.0f, 0.0f, 0.2f, 1.0f } };

        VkRenderPassBeginInfo renderPassBeginInfo = {};
        renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassBeginInfo.renderArea.extent.width = width;
        renderPassBeginInfo.renderArea.extent.height = height;
        renderPassBeginInfo.clearValueCount = 2;
        renderPassBeginInfo.pClearValues = clearValues;
        renderPassBeginInfo.renderPass = renderPass;
        renderPassBeginInfo.framebuffer = framebuffer;

        vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport = {};
        viewport.height = (float)height;
        viewport.width = (float)width;
        viewport.minDepth = (float)0.0f;
        viewport.maxDepth = (float)1.0f;
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

        // Update dynamic scissor state
        VkRect2D scissor = {};
        scissor.extent.width = width;
        scissor.extent.height = height;
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

        // Render scene
        vkCmdDraw(commandBuffer, 6, 1, 0, 0);
        vkCmdEndRenderPass(commandBuffer);

        VK_CHECK_RESULT(vkEndCommandBuffer(commandBuffer));

        submitWork(commandBuffer, queue);

        vkDeviceWaitIdle(device);
    }

    /*
    Copy framebuffer image to host visible image
    */
    
    const char* imagedata;
    {
        // Create the linear tiled destination image to copy to and to read the memory from
        VkImageCreateInfo imgCreateInfo(vks::initializers::imageCreateInfo());
        imgCreateInfo.imageType = VK_IMAGE_TYPE_2D;
        imgCreateInfo.format = VK_FORMAT_R32G32B32A32_SFLOAT;
        imgCreateInfo.extent.width = width;
        imgCreateInfo.extent.height = height;
        imgCreateInfo.extent.depth = 1;
        imgCreateInfo.arrayLayers = 1;
        imgCreateInfo.mipLevels = 1;
        imgCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imgCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imgCreateInfo.tiling = VK_IMAGE_TILING_LINEAR;
        imgCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        // Create the image
        VkImage dstImage;
        VK_CHECK_RESULT(vkCreateImage(device, &imgCreateInfo, nullptr, &dstImage));
        // Create memory to back up the image
        VkMemoryRequirements memRequirements;
        VkMemoryAllocateInfo memAllocInfo(vks::initializers::memoryAllocateInfo());
        VkDeviceMemory dstImageMemory;
        vkGetImageMemoryRequirements(device, dstImage, &memRequirements);
        memAllocInfo.allocationSize = memRequirements.size;
        // Memory must be host visible to copy from
        memAllocInfo.memoryTypeIndex = ctx.findMemoryType(0, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, {}, memRequirements.memoryTypeBits);
        std::cout << "host visible memory index: " << memAllocInfo.memoryTypeIndex << std::endl;
        VK_CHECK_RESULT(vkAllocateMemory(device, &memAllocInfo, nullptr, &dstImageMemory));
        VK_CHECK_RESULT(vkBindImageMemory(device, dstImage, dstImageMemory, 0));

        // Do the actual blit from the offscreen image to our host visible destination image
        VkCommandBufferAllocateInfo cmdBufAllocateInfo = vks::initializers::commandBufferAllocateInfo(commandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1);
        VkCommandBuffer copyCmd;
        VK_CHECK_RESULT(vkAllocateCommandBuffers(device, &cmdBufAllocateInfo, &copyCmd));
        VkCommandBufferBeginInfo cmdBufInfo = vks::initializers::commandBufferBeginInfo();
        VK_CHECK_RESULT(vkBeginCommandBuffer(copyCmd, &cmdBufInfo));

        // Transition destination image to transfer destination layout
        insertImageMemoryBarrier(
            copyCmd,
            dstImage,
            0,
            VK_ACCESS_TRANSFER_WRITE_BIT,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 });

        // colorImage is already in VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, and does not need to be transitioned

        VkImageCopy imageCopyRegion{};
        imageCopyRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageCopyRegion.srcSubresource.layerCount = 1;
        imageCopyRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageCopyRegion.dstSubresource.layerCount = 1;
        imageCopyRegion.extent.width = width;
        imageCopyRegion.extent.height = height;
        imageCopyRegion.extent.depth = 1;

        vkCmdCopyImage(
            copyCmd,
            image.handle(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &imageCopyRegion);

        // Transition destination image to general layout, which is the required layout for mapping the image memory later on
        insertImageMemoryBarrier(
            copyCmd,
            dstImage,
            VK_ACCESS_TRANSFER_WRITE_BIT,
            VK_ACCESS_MEMORY_READ_BIT,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_IMAGE_LAYOUT_GENERAL,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 });

        VK_CHECK_RESULT(vkEndCommandBuffer(copyCmd));

        submitWork(copyCmd, queue);

        // Get layout of the image (including row pitch)
        VkImageSubresource subResource{};
        subResource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        VkSubresourceLayout subResourceLayout;

        vkGetImageSubresourceLayout(device, dstImage, &subResource, &subResourceLayout);

        // Map image memory so we can start copying from it
        vkMapMemory(device, dstImageMemory, 0, VK_WHOLE_SIZE, 0, (void**)&imagedata);
        imagedata += subResourceLayout.offset;
        std::cout << "offset " << subResourceLayout.offset << std::endl;

#if defined (VK_USE_PLATFORM_ANDROID_KHR)
        const char* filename = strcat(getenv("EXTERNAL_STORAGE"), "/headless.ppm");
#else
        const char* filename = "headless.ppm";
#endif
        std::ofstream file(filename, std::ios::out | std::ios::binary);

        // ppm header
        file << "P6\n" << width << "\n" << height << "\n" << 255 << "\n";

        // If source is BGR (destination is always RGB) and we can't use blit (which does automatic conversion), we'll have to manually swizzle color components
        // Check if source is BGR and needs swizzle
        std::vector<VkFormat> formatsBGR = { VK_FORMAT_B8G8R8A8_SRGB, VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_B8G8R8A8_SNORM };
        const bool colorSwizzle = (std::find(formatsBGR.begin(), formatsBGR.end(), VK_FORMAT_R8G8B8A8_UNORM) != formatsBGR.end());

        // ppm binary pixel data
        std::cout << "rowpitch " << subResourceLayout.rowPitch << std::endl;
        for (int32_t y = 0; y < height; y++) {
            float *row = (float*)imagedata;
            for (int32_t x = 0; x < width; x++) {
                unsigned char r = unsigned char(*row * 255);
                unsigned char g = unsigned char(*(row+1) * 255);
                unsigned char b = unsigned char(*(row+2) * 255);
                file.write(reinterpret_cast<char*>(&r), 1);
                file.write(reinterpret_cast<char*>(&g), 1);
                file.write(reinterpret_cast<char*>(&b), 1);
                row += 4;
            }
            imagedata += subResourceLayout.rowPitch;
        }
        file.close();

        std::cout << "Framebuffer image saved to " << filename << std::endl;

        // Clean up resources
        vkUnmapMemory(device, dstImageMemory);
        vkFreeMemory(device, dstImageMemory, nullptr);
        vkDestroyImage(device, dstImage, nullptr);
    }

    vkQueueWaitIdle(queue);

    VkwMemoryHelper::GPUToImage(image).write("assets/vulkan/test_generator.png");
    std::cout << "Image was written" << std::endl;
}
