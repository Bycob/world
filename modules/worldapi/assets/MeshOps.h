#pragma once

#include "core/WorldConfig.h"

#include "Mesh.h"

namespace world {

class WORLDAPI_EXPORT MeshOps {
public:
    MeshOps() = delete;

    /** Calculates all the normals of the vertices in the mesh,
     * based on face normals. The normal of a vertex is the normalized
     * sum of the normals of the contiguous faces. */
    static void recalculateNormals(Mesh &mesh);
};
} // namespace world
