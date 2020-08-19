#include "VkwTrunkTexture.h"

#include <world/tree/Tree.h>
#include <world/assets/ImageUtils.h>

namespace world {

WORLD_REGISTER_CHILD_CLASS(ITreeWorker, VkwTrunkTexture, "VkwTrunkTexture")

VkwTrunkTexture::VkwTrunkTexture(size_t texSize, const std::string &shaderName)
        : _texSize(texSize), _texGen(texSize, texSize, shaderName) {}

void VkwTrunkTexture::processTree(Tree &tree, double resolution) {
    if (tree.isTwoMeshes(resolution)) {
        auto &trunkTex = tree.getTrunkTexture();

        // Check if the texture is already generated
        if (trunkTex.width() * trunkTex.height() > 1)
            return;

        trunkTex = Image(_texSize, _texSize, ImageType::RGB);
        // TODO make texture generator usable multiple times
        _texGen.generateTextureAsync();
        _texGen.getGeneratedImage(trunkTex);
    }
}

// TODO transmit parameters
VkwTrunkTexture *VkwTrunkTexture::clone() const {
    return new VkwTrunkTexture(_texSize);
}

} // namespace world