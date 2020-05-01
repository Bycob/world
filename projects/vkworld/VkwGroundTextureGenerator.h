#ifndef VKWORLD_VKWGROUNDTEXTUREGENERATOR_H
#define VKWORLD_VKWGROUNDTEXTUREGENERATOR_H

#include "VkWorldConfig.h"

#include <world/assets/Image.h>
#include <world/terrain/MultilayerGroundTexture.h>

#include <vkworld/wrappers/VkwImage.h>

namespace world {

class VkwGroundTextureGeneratorPrivate;
class LodTextures;

class VKWORLD_EXPORT VkwGroundTextureGenerator : public ITextureProvider {
    WORLD_WRITE_SUBCLASS_METHOD
public:
    VkwGroundTextureGenerator();
    ~VkwGroundTextureGenerator() override;

    VkwGroundTextureGenerator(const VkwGroundTextureGenerator &) = delete;
    VkwGroundTextureGenerator &operator=(const VkwGroundTextureGenerator &) =
        delete;

    void addLayer(const std::string &textureShader);

    size_t getLayerCount();

    VkwImage &getVkTexture(int layer, int lod);

    Image &getTexture(int layer, int lod) override;

    void setBaseWorldWidth(float width) { _baseWorldWidth = width; }

    void write(WorldFile &wf) const override;

    void read(const WorldFile &wf) override;

private:
    VkwGroundTextureGeneratorPrivate *_internal;

    float _baseWorldWidth = 100;


    float getWorldWidth(int lod);

    /// @param cpu Si true, récupère les textures sur le cpu depuis la carte
    /// graphique
    LodTextures &getOrCreate(int lod, bool cpu = false);
};

} // namespace world

#endif // VKWORLD_VKWGROUNDTEXTUREGENERATOR_H
