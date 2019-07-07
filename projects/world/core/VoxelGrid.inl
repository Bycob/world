#include "VoxelGrid.h"

#include <unordered_map>
#include <map>

namespace {
#include <transvoxel/Transvoxel.cpp>
}

namespace world {

template <typename data_t>
inline void VoxelGrid<data_t>::fromMesh(const Mesh &mesh) {
    for (u32 z = 0; z < _dims.z; ++z) {
        for (u32 y = 0; y < _dims.y; ++y) {
            for (u32 x = 0; x < _dims.x; ++x) {
            }
        }
    }
}


namespace voxels {
const vec3u X{1, 0, 0};
const vec3u Y{0, 1, 0};
const vec3u Z{0, 0, 1};

struct Edge {
    vec3u origin;
    vec3u direction;
};

inline bool operator<(const Edge &lhs, const Edge &rhs) {
    return lhs.origin < rhs.origin
               ? true
               : (lhs.origin == rhs.origin && lhs.direction < rhs.direction);
}

const std::unordered_map<u8, Edge> EDGES{
    {0x01, {0, X}}, {0x02, {0, Y}}, {0x13, {X, Y}},     {0x23, {Y, X}},
    {0x45, {Z, X}}, {0x46, {Z, Y}}, {0x57, {X + Z, Y}}, {0x67, {Y + Z, X}},
    {0x04, {0, Z}}, {0x15, {X, Z}}, {0x26, {Y, Z}},     {0x37, {X + Y, Z}},
};

} // namespace voxels

template <typename data_t>
inline u8 VoxelGrid<data_t>::getMarchingCubePolicy(
    const world::vec3u &i) const {
    using namespace voxels;
    return (at(i) > 0) | ((at(i + X) > 0) << 1) | ((at(i + Y) > 0) << 2) |
           ((at(i + X + Y) > 0) << 3) | ((at(i + Z) > 0) << 4) |
           ((at(i + X + Z) > 0) << 5) | ((at(i + Y + Z) > 0) << 6) |
           ((at(i + X + Y + Z) > 0) << 7);
}

template <typename data_t>
inline void VoxelGrid<data_t>::fillMesh(Mesh &mesh,
                                        const BoundingBox &bbox) const {
    using namespace voxels;
    std::map<Edge, u32> vertices;
    vec3u i;

    auto addVert = [this, &mesh, &i, &vertices, &bbox](const vec3u &origin,
                                                       const vec3u &axis) {
        vec3u c = i + origin;
        vec3u d = c + axis;
        data_t cval = at(c);
        data_t dval = at(d);

        // Edge case: If one value is equal to 0, we still create a vertex
        // because a face may be created using this vertex.
        if (cval * dval <= 0) {
            double p = 0.5; // (0 - cval) / (dval - cval);
            vec3d newVert = c * (1 - p) + d * p;
            newVert = newVert / (_dims - vec3d{1}) * bbox.getDimensions() +
                      bbox.getLowerBound();
            vertices[{c, axis}] = mesh.getVerticesCount();
            mesh.newVertex(newVert);
        }
    };

    for (u32 z = 0; z < _dims.z - 1; ++z) {
        for (u32 y = 0; y < _dims.y - 1; ++y) {
            for (u32 x = 0; x < _dims.x - 1; ++x) {
                i = {x, y, z};

                // Add vertices
                if (x == 0) {
                    addVert(Z, Y);
                    addVert(Y, Z);
                }

                if (y == 0) {
                    addVert(Z, X);
                    addVert(X, Z);

                    if (x == 0) {
                        addVert(0, Z);
                    }
                }

                if (z == 0) {
                    addVert(Y, X);
                    addVert(X, Y);

                    if (y == 0) {
                        addVert(0, X);
                    }

                    if (x == 0) {
                        addVert(0, Y);
                    }
                }

                addVert(Y + Z, X);
                addVert(X + Z, Y);
                addVert(X + Y, Z);

                // Add faces
                u8 policy = getMarchingCubePolicy(i);
                u8 caseIndex = regularCellClass[policy];
                auto caseData = regularCellData[caseIndex];
                auto vertexData = regularVertexData[policy];

                for (u8 t = 0; t < caseData.GetTriangleCount(); ++t) {
                    int ids[3];

                    for (u8 v = 0; v < 3; ++v) {
                        u8 vid = caseData.vertexIndex[t * 3 + v];
                        Edge edge = EDGES.at(vertexData[vid] & 0xFF);
                        edge.origin += i;
                        ids[v] = vertices.at(edge);
                    }

                    mesh.newFace(ids);
                }
            }
        }
    }

    MeshOps::recalculateNormals(mesh);
}
} // namespace world
