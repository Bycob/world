#ifndef VKWORLD_VKWGROUNDTEXTUREGENERATOR_H
#define VKWORLD_VKWGROUNDTEXTUREGENERATOR_H

#include "VkWorldConfig.h"

#include <world/assets/Image.h>

#include <vkworld/wrappers/VkwImage.h>

namespace world {

class VkwGroundTextureGeneratorPrivate;

class VKWORLD_EXPORT VkwGroundTextureGenerator {
public:
    VkwGroundTextureGenerator();
    ~VkwGroundTextureGenerator();

    VkwGroundTextureGenerator(const VkwGroundTextureGenerator &) = delete;
    VkwGroundTextureGenerator &operator=(const VkwGroundTextureGenerator &) =
        delete;

    void addLayer(const std::string &textureShader);

    size_t getLayerCount();

    VkwImage &getVkTexture(int layer, int lod);

    Image &getTexture(int layer, int lod);

    float _realWidth; // TODO this variable changes and makes no sense :P
private:
    VkwGroundTextureGeneratorPrivate *_internal;
};

} // namespace world

#endif // VKWORLD_VKWGROUNDTEXTUREGENERATOR_H
