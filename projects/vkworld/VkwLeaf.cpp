#include "VkwLeaf.h"

#include <world/assets/Image.h>
#include <world/tree/Tree.h>
#include <world/math/Bezier.h>

#include "wrappers/VkwTextureGenerator.h"
#include "wrappers/VkwMemoryHelper.h"

namespace world {

class VkwLeafPrivate {
public:
};

struct VkwLeaf::GeneratingLeaf {
    std::unique_ptr<VkwTextureGenerator> _generator;
};


WORLD_REGISTER_CHILD_CLASS(ITreeWorker, VkwLeaf, "VkwLeafTexture")

VkwLeaf::VkwLeaf() : _internal(new VkwLeafPrivate()) {}

VkwLeaf::~VkwLeaf() { delete _internal; }

void VkwLeaf::processTree(Tree &tree) {
    tree.getLeavesTexture() = generateLeafTexture();
}

Image VkwLeaf::generateLeafTexture() {
    return getTexture(generateLeafTextureAsync());
}

VkwLeaf::GeneratingLeaf *VkwLeaf::generateLeafTextureAsync() {
    auto *handle = new GeneratingLeaf{nullptr};
    handle->_generator = std::make_unique<VkwTextureGenerator>(
        _texSize, _texSize, "leaves.frag");
    handle->_generator->mesh() = createLeafMesh();
    handle->_generator->generateTextureAsync();
    return handle;
}

Image VkwLeaf::getTexture(VkwLeaf::GeneratingLeaf *handle) {
    Image image(_texSize, _texSize, ImageType::RGBA);
    handle->_generator->getGeneratedImage(image);
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

VkwLeaf *VkwLeaf::clone() const { return new VkwLeaf(); }

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
        u32 startID = mesh.getVerticesCount();

        BoundingBox bbox = getLeafBbox(i);
        // convert [0,1] to [-1,1]
        vec3d upper = bbox.getUpperBound() * 2 - vec3d{1};
        vec3d lower = bbox.getLowerBound() * 2 - vec3d{1};
        vec3d dims = upper - lower;
        vec3d start{(upper.x + lower.x) / 2, lower.y, 0};

        // stem is a trapezoid
        vec3d X{1, 0, 0};
        vec3d Y{0, 1, 0};
        vec3d Z{0, 0, 1};
        double stemStartWidth = 0.01 * dims.x;
        double stemEndWidth = 0.008 * dims.x;
        double stemHeight = 0.1 * dims.y;
        mesh.newVertex(start - X * stemStartWidth);
        mesh.newVertex(start + X * stemStartWidth);
        mesh.newVertex(start - X * stemEndWidth + Y * stemHeight);
        mesh.newVertex(start + X * stemEndWidth + Y * stemHeight);
        mesh.newFace(startID, startID + 1, startID + 2);
        mesh.newFace(startID + 1, startID + 3, startID + 2);

        // leaf is defined by two bezier curves (currently mirrored)
        vec3d bodyStart = start + Y * stemHeight;
        vec3d bodyEnd = start + Y * 0.95 * dims.y;
        vec3d dirStart{-1.0, -0.3, 0.0};
        vec3d dirEnd{-0.1, -1.0, 0.0};
        BezierCurve curve1{bodyStart, bodyEnd, dirStart, dirEnd};
        BezierCurve curve2{bodyStart,
                           bodyEnd,
                           {-dirStart.x, dirStart.y, dirStart.z},
                           {-dirEnd.x, dirStart.y, dirStart.z}};

        u32 splits = 4;
        u32 vertPerSplit = 2;
        double total = splits * vertPerSplit + 1;
        u32 splitVert = mesh.getVerticesCount();

        // attach point on stem ("splitVert")
        for (u32 j = 0; j <= splits; ++j) {
            if (j == 0) {
                mesh.newVertex(bodyStart, Z);
            } else {
                u32 splitId = vertPerSplit * j + 1;
                double tEnd = splitId / total;
                vec3d splitEndVert = curve1.getPointAt(tEnd);
                double dist = splitEndVert.x - bodyStart.x;

                mesh.newVertex(splitEndVert - vec3d{1, 0.2, 0} * dist, Z);
            }
        }

        // first point
        mesh.newVertex(curve1.getPointAt(i), Z);

        // remaining part of the leaf
        for (u32 j = 0; j < splits; ++j) {
            // first triangle
            mesh.newFace(splitVert + j, mesh.getVerticesCount() - 1,
                         splitVert + j + 1);

            // all triangles
            for (u32 k = 0; k < vertPerSplit; ++k) {
                u32 x = vertPerSplit * j + k + 2;
                double t = x / total;

                mesh.newVertex(curve1.getPointAt(t), Z);
                mesh.newFace(splitVert + j + 1, mesh.getVerticesCount() - 2,
                             mesh.getVerticesCount() - 1);
            }
        }
    }

    return mesh;
}

} // namespace world
