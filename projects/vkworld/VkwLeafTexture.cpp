#include "VkwLeafTexture.h"

#include <world/assets/Image.h>
#include <world/tree/Tree.h>
#include <world/math/Bezier.h>

#include "wrappers/VkwTextureGenerator.h"
#include "wrappers/VkwMemoryHelper.h"

namespace world {

class VkwLeafPrivate {
public:
};

struct VkwLeafTexture::GeneratingLeaf {
    std::unique_ptr<VkwTextureGenerator> _generator;
};


WORLD_REGISTER_CHILD_CLASS(ITreeWorker, VkwLeafTexture, "VkwLeafTexture")

VkwLeafTexture::VkwLeafTexture()
        : _internal(new VkwLeafPrivate()), _grid(_count),
          _mainColor(0.4, 0.9, 0.4) {}

VkwLeafTexture::~VkwLeafTexture() { delete _internal; }

void VkwLeafTexture::processTree(Tree &tree, double resolution) {
    if (tree.isTwoMeshes(resolution)) {
        auto &leavesTex = tree.getLeavesTexture();

        // Check if already generated
        if (leavesTex.width() * leavesTex.height() > 1)
            return;

        _grid = tree.getLeavesGrid();
        leavesTex = generateLeafTexture();
    }
}

Image VkwLeafTexture::generateLeafTexture() {
    return getTexture(generateLeafTextureAsync());
}

VkwLeafTexture::GeneratingLeaf *VkwLeafTexture::generateLeafTextureAsync() {
    auto *handle = new GeneratingLeaf{nullptr};
    handle->_generator = std::make_unique<VkwTextureGenerator>(
        _texSize, _texSize, "leaves.frag");
    handle->_generator->mesh() = createLeafMesh();

    // Set generator parameters
    float leavesColor[] = {static_cast<float>(_mainColor._r),
                           static_cast<float>(_mainColor._g),
                           static_cast<float>(_mainColor._b)};
    handle->_generator->addParameter(0, DescriptorType::UNIFORM_BUFFER,
                                     MemoryUsage::CPU_WRITES,
                                     sizeof(leavesColor), leavesColor);

    handle->_generator->generateTextureAsync();
    return handle;
}

Image VkwLeafTexture::getTexture(VkwLeafTexture::GeneratingLeaf *handle) {
    Image image(_texSize, _texSize, ImageType::RGBA);
    handle->_generator->getGeneratedImage(image);
    delete handle;
    return image;
}

VkwLeafTexture *VkwLeafTexture::clone() const { return new VkwLeafTexture(); }

void VkwLeafTexture::write(WorldFile &wf) const {
    wf.addUint("texSize", _texSize);
    wf.addUint("count", _count);
    wf.addStruct("mainColor", _mainColor);
}

void VkwLeafTexture::read(const WorldFile &wf) {
    wf.readUintOpt("texSize", _texSize);
    wf.readUintOpt("count", _count);
    wf.readStructOpt("mainColor", _mainColor);
}

Mesh VkwLeafTexture::createLeafMesh() {
    Mesh mesh;

    for (u32 i = 0; i < _count; ++i) {
        u32 startID = mesh.getVerticesCount();

        BoundingBox bbox = _grid.getBbox(i);
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
        vec3d dirStart = vec3d{-0.5, 0.05, 0.0} * dims;
        vec3d dirEnd = vec3d{-0.05, -0.1, 0.0} * dims;
        BezierCurve curve1{bodyStart, bodyEnd, dirStart, dirEnd};
        BezierCurve curve2{bodyStart,
                           bodyEnd,
                           {-dirStart.x, dirStart.y, dirStart.z},
                           {-dirEnd.x, dirEnd.y, dirEnd.z}};

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
                vec3d attachPoint = splitEndVert - vec3d{1, 0.2, 0} * dist;

                if (attachPoint.y < bodyStart.y) {
                    attachPoint.y = bodyStart.y;
                }

                mesh.newVertex(attachPoint, Z);
            }
        }

        // first point
        mesh.newVertex(curve1.getPointAt(1 / total), Z);
        mesh.newVertex(curve2.getPointAt(1 / total), Z);

        // remaining part of the leaf
        for (u32 j = 0; j < splits; ++j) {
            // first triangle
            mesh.newFace(splitVert + j, mesh.getVerticesCount() - 2,
                         splitVert + j + 1);
            mesh.newFace(splitVert + j, mesh.getVerticesCount() - 1,
                         splitVert + j + 1);

            // all triangles
            for (u32 k = 0; k < vertPerSplit; ++k) {
                u32 x = vertPerSplit * j + k + 2;
                double t = x / total;

                mesh.newVertex(curve1.getPointAt(t), Z);
                mesh.newVertex(curve2.getPointAt(t), Z);
                mesh.newFace(splitVert + j + 1, mesh.getVerticesCount() - 4,
                             mesh.getVerticesCount() - 2);
                mesh.newFace(splitVert + j + 1, mesh.getVerticesCount() - 3,
                             mesh.getVerticesCount() - 1);
            }
        }
    }

    return mesh;
}

} // namespace world
