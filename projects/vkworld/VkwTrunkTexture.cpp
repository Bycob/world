#include "VkwTrunkTexture.h"

#include <world/tree/Tree.h>
#include <world/assets/ImageUtils.h>

namespace world {

WORLD_REGISTER_CHILD_CLASS(ITreeWorker, VkwTrunkTexture, "VkwTrunkTexture")

VkwTrunkTexture::VkwTrunkTexture(size_t texSize, const std::string &shaderName)
        : _texSize(texSize), _color(0.35, 0.25, 0.1),
          _texGen(texSize, texSize, shaderName) {}

void VkwTrunkTexture::processTree(Tree &tree, double resolution) {
    if (tree.isTwoMeshes(resolution)) {
        auto &trunkTex = tree.getTrunkTexture();

        // Check if the texture is already generated
        if (trunkTex.width() * trunkTex.height() > 1)
            return;

        // Set generator parameters
        float trunkColor[] = {static_cast<float>(_color._r),
                              static_cast<float>(_color._g),
                              static_cast<float>(_color._b)};
        _texGen.addParameter(0, DescriptorType::UNIFORM_BUFFER,
                             MemoryUsage::CPU_WRITES, sizeof(trunkColor),
                             trunkColor);

        trunkTex = Image(_texSize, _texSize, ImageType::RGB);
        // TODO make texture generator usable multiple times
        _texGen.generateTextureAsync();
        _texGen.getGeneratedImage(trunkTex);
    }
}

// TODO transmit parameters
VkwTrunkTexture *VkwTrunkTexture::clone() const {
    auto *copy = new VkwTrunkTexture(_texSize);
    copy->_color = _color;
    return copy;
}

void VkwTrunkTexture::write(WorldFile &wf) const {
    wf.addUint("texSize", _texSize);
    wf.addStruct("color", _color);
}

void VkwTrunkTexture::read(const WorldFile &wf) {
    wf.readUintOpt("texSize", _texSize);
    wf.readStructOpt("color", _color);
}

} // namespace world