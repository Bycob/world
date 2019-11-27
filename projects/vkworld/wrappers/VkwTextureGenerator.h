#ifndef VKWORLD_VKWTEXTUREGENERATOR_H
#define VKWORLD_VKWTEXTUREGENERATOR_H

#include "vkworld/VkWorldConfig.h"

#include <memory>
#include <map>

#include <world/assets/Image.h>
#include <world/assets/Mesh.h>

#include "VkwRenderPass.h"
#include "VkwGraphicsPipeline.h"
#include "VkwWorker.h"
#include "VkwSubBuffer.h"

namespace world {

class VKWORLD_EXPORT VkwTextureGenerator {
public:
    VkwTextureGenerator(int width, int height, std::string shaderName);

    ~VkwTextureGenerator();

    void addParameter(int id, DescriptorType type, MemoryUsage memtype,
                      size_t size, void *data);

    void addImageParameter(int id, const VkwImage &image);

    Image generateTexture();

    void generateTextureAsync();

    /** Wait for the end of the generation, then retrieves the generated
     * image. */
    Image getGeneratedImage();

    void getGeneratedImage(Image &image);

    Mesh &mesh() { return _mesh; }

private:
    int _width, _height;
    std::string _shaderName;
    Mesh _mesh;

    VkwDescriptorSetLayout _layout;
    std::map<int, VkwSubBuffer> _buffers;
    std::map<int, VkwImage> _images;

    std::unique_ptr<VkwGraphicsWorker> _worker;
    VkwSubBuffer _indicesBuf;
    VkwSubBuffer _verticesBuf;
    VkwImage _texture;
};

} // namespace world

#endif // VKWORLD_VKWTEXTUREGENERATOR_H
