#include "common.h"

#include <world/core.h>

using namespace world;

extern "C" {

struct MaterialDescription {
    char *MapKd;
    double Kdr, Kdg, Kdb;
    double Ksr, Ksg, Ksb;
    bool transparent;
};

#define DOUBLE_VERTEX_SIZE (sizeof(Vertex) / sizeof(double))

PEACE_EXPORT void readMeshSizes(MeshPtr meshPtr, int *vertSize,
                                int *indicesSize) {
    auto *mesh = static_cast<Mesh *>(meshPtr);
    *vertSize = mesh->getVerticesCount() * DOUBLE_VERTEX_SIZE;
    *indicesSize = mesh->getFaceCount() * 3;
}

PEACE_EXPORT void readMesh(MeshPtr meshPtr, double *vertices, int *indices) {
    auto *mesh = static_cast<Mesh *>(meshPtr);

    for (u32 i = 0; i < mesh->getVerticesCount(); ++i) {
        Vertex &vertex = mesh->getVertex(i);
        std::memcpy(vertices + i * DOUBLE_VERTEX_SIZE, &vertex, sizeof(Vertex));
    }

    for (u32 i = 0; i < mesh->getFaceCount(); ++i) {
        Face &face = mesh->getFace(i);
        indices[i * 3 + 0] = face.getID(0);
        indices[i * 3 + 1] = face.getID(1);
        indices[i * 3 + 2] = face.getID(2);
    }
}

PEACE_EXPORT MaterialDescription readMaterial(MaterialPtr materialPtr) {
    auto *material = static_cast<Material *>(materialPtr);

    MaterialDescription result{};
    result.MapKd = strdup(material->getMapKd().c_str());

    Color4d kd = material->getKd();
    result.Kdr = kd._r;
    result.Kdg = kd._g;
    result.Kdb = kd._b;

    Color4d ks = material->getKs();
    result.Ksr = ks._r;
    result.Ksg = ks._g;
    result.Ksb = ks._b;

    result.transparent = material->isTransparent();

    return result;
}

PEACE_EXPORT void readTexture(TexturePtr texturePtr, u8 **data, int *width,
                              int *height, int *type) {
    auto *image = static_cast<Image *>(texturePtr);
    *width = image->width();
    *height = image->height();
    *type = image->elemSize();
    *data = reinterpret_cast<u8 *>(&image->grey(0, 0));
}
}