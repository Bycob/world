#include "VkwTextureGenerator.h"


#include "VkwMemoryHelper.h"

namespace world {
VkwTextureGenerator::VkwTextureGenerator(int width, int height,
                                         std::string shaderName)
        : _width(width), _height(height),
          _texture(width, height, VkwImageUsage::OFFSCREEN_RENDER,
                   vk::Format::eR32G32B32A32Sfloat),
          _shaderName(std::move(shaderName)) {
    _worker = std::make_unique<VkwGraphicsWorker>();
}

VkwTextureGenerator::~VkwTextureGenerator() = default;

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
    _images.emplace(id, image);
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

    auto &ctx = Vulkan::context();
    VkwRenderPass renderPass(_texture);
    pipeline.setRenderPass(renderPass);

    _worker->beginRenderPass(renderPass);
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
    Image img(_width, _height, ImageType::RGB);
    getGeneratedImage(img);
    return img;
}

void VkwTextureGenerator::getGeneratedImage(Image &img) {
    _worker->waitForCompletion();
    VkwMemoryHelper::GPUToImage(_texture, img, 4);
}
} // namespace world
