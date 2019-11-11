#pragma once

#include "world/core/WorldConfig.h"

#include "Mesh.h"

namespace world {

class WORLDAPI_EXPORT MeshOps {
public:
    MeshOps() = delete;

    /** Calculates all the normals of the vertices in the mesh,
     * based on face normals. The normal of a vertex is the normalized
     * sum of the normals of the contiguous faces. */
    static void recalculateNormals(Mesh &mesh);

    /** Add every faces and vertices from mesh `src` to mesh `dst`. */
    static void addAll(Mesh &dst, const Mesh &src);

    /** Utility function to concatenate more than two meshes inplace. See
     * #addAll() for details. */
    template <typename... Meshes>
    static void addAll(Mesh &dst, const Mesh &src1, const Meshes &... srcs) {
        addAll(dst, src1);
        addAll(dst, srcs...);
    }

    /** Create one mesh containing all the vertices and faces of the two given
     * meshes. */
    static Mesh concatMeshes(const Mesh &mesh1, const Mesh &mesh2);

    /** Utility function to concatenate more than two meshes. See #concatMeshes
     * for details. */
    template <typename... Meshes>
    static Mesh concatMeshes(const Mesh &mesh1, const Mesh &mesh2,
                             const Meshes &... meshes) {
        Mesh mesh12 = concatMeshes(mesh1, mesh2);
        addAll(mesh12, meshes...);
        return mesh12;
    }
};
} // namespace world
