#include "MeshOps.h"

#include "world/math/MathsHelper.h"

namespace world {

void MeshOps::recalculateNormals(Mesh &mesh) {
    std::vector<vec3d> normalSum(mesh.getVerticesCount(), vec3d());
    std::vector<int> normalCount(mesh.getVerticesCount(), 0);

    for (u32 i = 0; i < mesh.getFaceCount(); i++) {
        const Face &face = mesh.getFace(i);
        const int count = face.vertexCount();

        for (int j = 0; j < count; j++) {
            // Calcul de la normale de la face par rapport au point j
            int id0 = face.getID(j);
            int id1 = face.getID(mod(j + 1, count));
            int id2 = face.getID(mod(j - 1, count));

            auto pt1 = mesh.getVertex(id0).getPosition();
            auto pt2 = mesh.getVertex(id1).getPosition();
            auto pt3 = mesh.getVertex(id2).getPosition();

            vec3d v1 = pt2 - pt1;
            vec3d v2 = pt3 - pt1;
            vec3d normal = v1.crossProduct(v2).normalize();

            // Ajout de la normale au calcul de la moyenne
            vec3d &inplace = normalSum.at(id0);
            inplace = inplace + normal;

            normalCount.at(id0)++;
        }
    }

    // On set les normales
    for (u64 i = 0; i < normalSum.size(); i++) {
        int count = normalCount.at(i);
        vec3d normal;

        if (count != 0) {
            normal = normalSum.at(i) / count;
        } else {
            normal = vec3d(0, 0, 1);
        }

        Vertex &vn = mesh.getVertex(i);
        vn.setNormal(normal);
    }
}

void MeshOps::scale(Mesh &mesh, vec3d scaleFactor) {
    for (u32 i = 0; i < mesh.getVerticesCount(); ++i) {
        Vertex &vert = mesh.getVertex(i);
        vert.setPosition(vert.getPosition() * scaleFactor);
        vert.setNormal((vert.getNormal() * scaleFactor).normalize());
    }
}

void MeshOps::addAll(Mesh &dst, const Mesh &src) {
    int offset = dst.getVerticesCount();

    for (u32 i = 0; i < src.getVerticesCount(); ++i) {
        dst.addVertex(src.getVertex(i));
    }

    for (u32 i = 0; i < src.getFaceCount(); ++i) {
        Face f = src.getFace(i);

        for (int j = 0; j < f.vertexCount(); ++j) {
            f.setID(j, f.getID(j) + offset);
        }

        dst.addFace(f);
    }
}

Mesh MeshOps::concatMeshes(const Mesh &mesh1, const Mesh &mesh2) {
    Mesh concat = mesh1;
    addAll(concat, mesh2);
    return concat;
}

} // namespace world
