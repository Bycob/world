#ifndef WORLD_VKWTRUNKTEXTURE_H
#define WORLD_VKWTRUNKTEXTURE_H

#include "VkWorldConfig.h"

#include <world/tree/ITreeWorker.h>

#include "wrappers/VkwTextureGenerator.h"

namespace world {

class VKWORLD_EXPORT VkwTrunkTexture : public ITreeWorker {
    WORLD_WRITE_SUBCLASS_METHOD
public:
    VkwTrunkTexture(size_t texSize = 128,
                    const std::string &shaderName = "trunk.frag");

    void processTree(Tree &tree, double resolution) override;

    VkwTrunkTexture *clone() const override;

private:
    size_t _texSize;

    VkwTextureGenerator _texGen;
};

} // namespace world

#endif // WORLD_VKWTRUNKTEXTURE_H
