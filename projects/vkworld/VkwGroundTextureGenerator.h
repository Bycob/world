#ifndef VKWORLD_VKWGROUNDTEXTUREGENERATOR_H
#define VKWORLD_VKWGROUNDTEXTUREGENERATOR_H

#include "VkWorldConfig.h"

#include <world/assets/Image.h>
#include <world/terrain/MultilayerGroundTexture.h>

#include <vkworld/wrappers/VkwImage.h>

namespace world {

class VkwGroundTextureGeneratorPrivate;

class VKWORLD_EXPORT VkwGroundTextureGenerator : public ITextureProvider {
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

    void setBaseWorldWidth(float width) { _baseWorldWidth = width; }

private:
    VkwGroundTextureGeneratorPrivate *_internal;

    float _baseWorldWidth = 100;


    float getWorldWidth(int lod);
};

} // namespace world

#endif // VKWORLD_VKWGROUNDTEXTUREGENERATOR_H
