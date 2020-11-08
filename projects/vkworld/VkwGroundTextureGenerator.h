#ifndef VKWORLD_VKWGROUNDTEXTUREGENERATOR_H
#define VKWORLD_VKWGROUNDTEXTUREGENERATOR_H

#include "VkWorldConfig.h"

#include <world/assets/Image.h>
#include <world/terrain/MultilayerGroundTexture.h>
#include <world/terrain/GroundBiomes.h>

#include <vkworld/wrappers/VkwImage.h>

namespace world {

class VkwGroundTextureGeneratorPrivate;
class LodTextures;

class VKWORLD_EXPORT VkwGroundTextureGenerator : public ITextureProvider,
                                                 public IBiomeTextureGenerator {
    WORLD_WRITE_SUBCLASS_METHOD
public:
    VkwGroundTextureGenerator();
    ~VkwGroundTextureGenerator() override;

    VkwGroundTextureGenerator(const VkwGroundTextureGenerator &) = delete;
    VkwGroundTextureGenerator &operator=(const VkwGroundTextureGenerator &) =
        delete;

    void addLayer(const std::string &textureShader);

    void addLayer(Shader shader);

    void addLayer(const BiomeLayer &layer) override;

    size_t getLayerCount();

    VkwImage &getVkTexture(int layer, int lod);

    Image &getTexture(int layer, int lod) override;

    void write(WorldFile &wf) const override;

    void read(const WorldFile &wf) override;

private:
    VkwGroundTextureGeneratorPrivate *_internal;


    /// Get world size of a tile
    float getWidth(int lod);

    /// @param cpu Si true, récupère les textures sur le cpu depuis la carte
    /// graphique
    LodTextures &getOrCreate(int layerId, int lod, bool cpu);
};

} // namespace world

#endif // VKWORLD_VKWGROUNDTEXTUREGENERATOR_H
