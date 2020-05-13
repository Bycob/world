#ifndef WORLD_VKWTRUNKTEXTURE_H
#define WORLD_VKWTRUNKTEXTURE_H

#include "VkWorldConfig.h"

#include <world/tree/ITreeWorker.h>

#include "wrappers/VkwTextureGenerator.h"

namespace world {

class VKWORLD_EXPORT VkwTrunkTexture : public ITreeWorker {
    WORLD_WRITE_SUBCLASS_METHOD
public:
    VkwTrunkTexture(const std::string &shaderName = "trunk.frag");

    void process(TreeInstance &tree) override;

    VkwTrunkTexture *clone() const override;

private:
    VkwTextureGenerator _texGen;
};

} // namespace world

#endif // WORLD_VKWTRUNKTEXTURE_H
