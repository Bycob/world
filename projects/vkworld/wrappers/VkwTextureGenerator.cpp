#include "VkwTextureGenerator.h"


#include "VkwMemoryHelper.h"

namespace world {
VkwTextureGenerator::VkwTextureGenerator(int width, int height,
                                         std::string shaderName)
        : _width(width), _height(height),
          _texture(VkwImageUsage::OFFSCREEN_RENDER,
                   vk::Format::eR32G32B32A32Sfloat, width, height),
          _shaderName(std::move(shaderName)) {
    _worker = std::make_unique<VkwGraphicsWorker>();
}

VkwTextureGenerator::~VkwTextureGenerator() {
    auto &ctx = Vulkan::context();
    ctx._device.destroy(_renderPass);
    ctx._device.destroy(_framebuffer);
}

void VkwTextureGenerator::addParameter(int id, DescriptorType type,
                                       MemoryUsage memtype, size_t size,
                                       void *data) {
    _layout.addBinding(type, id);

    auto &ctx = Vulkan::context();
    VkwSubBuffer buffer = ctx.allocate(size, type, memtype);
    buffer.setData(data);
    _buffers[id] = buffer;
}

void VkwTextureGenerator::addImageParameter(int id, const VkwImage &image) {
    _layout.addBinding(DescriptorType::IMAGE, id);
    _images[id] = image;
}

Image VkwTextureGenerator::generateTexture() {
    generateTextureAsync();
    return getGeneratedImage();
}

void VkwTextureGenerator::generateTextureAsync() {
    VkwDescriptorSet dset(_layout);
    for (auto &entry : _buffers) {
        dset.addDescriptor(entry.first, entry.second);
    }

    for (auto &entry : _images) {
        dset.addDescriptor(entry.first, entry.second);
    }

    VkwGraphicsPipeline pipeline(_layout);

    if (_mesh.empty()) {
        pipeline.setBuiltinShader(VkwShaderType::VERTEX,
                                  "generic_texture.vert");
        pipeline.enableVertexBuffer(false);
    } else {
        pipeline.setBuiltinShader(VkwShaderType::VERTEX, "generic_2D.vert");
    }
    pipeline.setBuiltinShader(VkwShaderType::FRAGMENT, _shaderName);

    // This is only one render pass
    // --- Create Render Pass
    auto &ctx = Vulkan::context();

    vk::AttachmentDescription colorAttachment({}, _texture.format());
    colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
    colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
    colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
    // TODO: ePresentSrcKHR if we need to display it to the presentation
    colorAttachment.finalLayout = vk::ImageLayout::eGeneral;

    vk::AttachmentReference attachmentRef(
        0, vk::ImageLayout::eColorAttachmentOptimal);
    vk::SubpassDescription subpass({}, vk::PipelineBindPoint::eGraphics, 0,
                                   nullptr, 1, &attachmentRef);

    vk::RenderPassCreateInfo renderPassInfo({}, 1, &colorAttachment, 1,
                                            &subpass);
    _renderPass = ctx._device.createRenderPass(renderPassInfo);

    pipeline.setDimensions(_width, _height);
    pipeline.setRenderPass(_renderPass);

    // --- Create framebuffer
    vk::ImageView imgView = _texture.getImageView();
    vk::FramebufferCreateInfo framebufferInfo({}, _renderPass, 1, &imgView,
                                              _width, _height, 1);
    _framebuffer = ctx._device.createFramebuffer(framebufferInfo);

    // --- Begin renderpass (with framebuffer)
    _worker->beginRenderPass(_renderPass, _framebuffer, _width, _height);
    // --- Bind descriptor set & pipeline
    _worker->bindCommand(pipeline, dset);

    // --- Draw
    if (_mesh.empty()) {
        _worker->draw(6);
    } else {
        // TODO Staging buffers
        // Setup Vertex buffer
        std::vector<VkwVertex> vertices;

        for (u32 i = 0; i < _mesh.getVerticesCount(); ++i) {
            vertices.emplace_back(_mesh.getVertex(i));
        }
        _verticesBuf = ctx.allocate(vertices.size() * sizeof(VkwVertex),
                                    DescriptorType::VERTEX_BUFFER,
                                    MemoryUsage::CPU_WRITES);
        _verticesBuf.setData(&vertices[0]);

        // Setup Indices buffer
        std::vector<u32> indices;

        for (u32 i = 0; i < _mesh.getFaceCount(); ++i) {
            const Face &face = _mesh.getFace(i);

            for (u32 j = 0; j < face.vertexCount(); ++j) {
                indices.push_back(face.getID(j));
            }
        }
        _indicesBuf =
            ctx.allocate(indices.size() * sizeof(u32),
                         DescriptorType::INDEX_BUFFER, MemoryUsage::CPU_WRITES);
        _indicesBuf.setData(&indices[0]);

        // Draw Indexed
        _worker->drawIndexed(_indicesBuf, _verticesBuf, indices.size());
    }
    // End renderpass
    _worker->endRenderPass();
    _worker->endCommandRecording();

    _worker->run();
}

Image VkwTextureGenerator::getGeneratedImage() {
    _worker->waitForCompletion();
    Image img(_width, _height, ImageType::RGB);
    VkwMemoryHelper::GPUToImage(_texture, img, 4);
    return img;
}
} // namespace world
