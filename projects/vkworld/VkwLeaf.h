#ifndef VKWORLD_VKWLEAF_H
#define VKWORLD_VKWLEAF_H

#include "VkWorldConfig.h"

#include <world/assets/Image.h>
#include <world/assets/Mesh.h>
#include <world/math/BoundingBox.h>
#include <world/tree/ITreeWorker.h>
#include <world/core/SpriteGrid.h>

namespace world {

class VkwLeafPrivate;

// TODO change name to VkwLeafTexture
class VKWORLD_EXPORT VkwLeaf : public ITreeWorker {
    WORLD_WRITE_SUBCLASS_METHOD
public:
    struct GeneratingLeaf;

    VkwLeaf();
    ~VkwLeaf() override;

    void processTree(Tree &tree) override;

    /** Generate one leaf texture. */
    Image generateLeafTexture();

    /** Start one leaf texture generation. Returns a handle to get
     * the resulting Leaf. */
    GeneratingLeaf *generateLeafTextureAsync();

    /** Get generated leaf from the given handle. The handle is
     * then discarded. */
    Image getTexture(GeneratingLeaf *handle);

    VkwLeaf *clone() const override;

private:
    VkwLeafPrivate *_internal;

    u32 _count = 7;
    u32 _texSize = 512;

    SpriteGrid _grid;

    Mesh createLeafMesh();
};

} // namespace world

#endif // VKWORLD_VKWLEAF_H
