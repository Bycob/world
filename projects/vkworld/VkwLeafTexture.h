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

class VKWORLD_EXPORT VkwLeafTexture : public ITreeWorker {
    WORLD_WRITE_SUBCLASS_METHOD
public:
    struct GeneratingLeaf;

    VkwLeafTexture();
    ~VkwLeafTexture() override;

    void processTree(Tree &tree, double resolution) override;

    /** Generate one leaf texture. */
    Image generateLeafTexture();

    /** Start one leaf texture generation. Returns a handle to get
     * the resulting Leaf. */
    GeneratingLeaf *generateLeafTextureAsync();

    /** Get generated leaf from the given handle. The handle is
     * then discarded. */
    Image getTexture(GeneratingLeaf *handle);

    VkwLeafTexture *clone() const override;

    void write(WorldFile &wf) const override;

    void read(const WorldFile &wf) override;

private:
    VkwLeafPrivate *_internal;

    /// Number of leaves on the texture
    u32 _count = 7;
    /// Resolution of the texture
    u32 _texSize = 512;

    SpriteGrid _grid;

    Color4d _mainColor;

    Mesh createLeafMesh();
};

} // namespace world

#endif // VKWORLD_VKWLEAF_H
