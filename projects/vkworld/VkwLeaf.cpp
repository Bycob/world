#include "VkwLeaf.h"

#include <world/assets/Image.h>

#include "wrappers/VkwWorker.h"
#include "wrappers/VkwMemoryHelper.h"

namespace world {

class VkwLeafPrivate {
public:
};

struct VkwLeaf::GeneratingLeaf {
    std::unique_ptr<VkwGraphicsWorker> _worker;
    VkwImage _image;
};


VkwLeaf::VkwLeaf() : _internal(new VkwLeafPrivate()) {}

VkwLeaf::~VkwLeaf() { delete _internal; }

Image VkwLeaf::generateLeafTexture() {
    return getTexture(generateLeafTextureAsync());
}

VkwLeaf::GeneratingLeaf *VkwLeaf::generateLeafTextureAsync() {
    GeneratingLeaf *handle = new GeneratingLeaf{nullptr, VkwImage(1, 1)};

    createLeafMesh();

    return handle;
}

Image VkwLeaf::getTexture(VkwLeaf::GeneratingLeaf *handle) {
    handle->_worker->waitForCompletion();
    Image image(1, 1, ImageType::RGBA);
    VkwMemoryHelper::GPUToImage(handle->_image, image);
    delete handle;
    return image;
}

BoundingBox VkwLeaf::getLeafBbox(u32 id) {
    u32 line = getLineCount();
    u32 x = id % line;
    u32 y = id / line;
    vec3d o(float(x) / line, float(y) / line, 0);

    BoundingBox bbox(o, o + vec3d{1. / line});
    return bbox;
}

u32 VkwLeaf::getLineCount() {
    u32 line = 0;
    while (line * line < _count) {
        ++line;
    }
    return line;
}

Mesh VkwLeaf::createLeafMesh() {
    Mesh mesh;

    for (u32 i = 0; i < _count; ++i) {
        BoundingBox bbox = getLeafBbox(i);
        vec3d upper = bbox.getUpperBound() * 2 - vec3d{1};
        vec3d lower = bbox.getLowerBound() * 2 - vec3d{1};
        vec3d start{(upper.x + lower.x) / 2, lower.y, 0};

        // TODO to be continued
    }

    return mesh;
}

} // namespace world
