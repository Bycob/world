#ifndef VKWORLD_VKWLEAF_H
#define VKWORLD_VKWLEAF_H

#include "VkWorldConfig.h"

#include <world/assets/Image.h>
#include <world/assets/Mesh.h>
#include <world/math/BoundingBox.h>

namespace world {

class VkwLeafPrivate;

class VKWORLD_EXPORT VkwLeaf {
public:
    struct GeneratingLeaf;

    VkwLeaf();
    ~VkwLeaf();

    /** Generate one leaf texture. */
    Image generateLeafTexture();

    /** Start one leaf texture generation. Returns a handle to get
     * the resulting Leaf. */
    GeneratingLeaf *generateLeafTextureAsync();

    /** Get generated leaf from the given handle. The handle is
     * then discarded. */
    Image getTexture(GeneratingLeaf *handle);

    BoundingBox getLeafBbox(u32 id);

private:
    VkwLeafPrivate *_internal;

    u32 _count;
    u32 _texSize;


    /** Get number of leaves on one line. */
    u32 getLineCount();

    Mesh createLeafMesh();
};

} // namespace world

#endif // VKWORLD_VKWLEAF_H
