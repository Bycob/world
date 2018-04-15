#include "Mesh.h"

#include <iostream>
#include <stdexcept>

#include "math/MathsHelper.h"

namespace world {

//----- FACE

Face::Face() : _ids{0, 0, 0} {}

Face::Face(int *ids) : _ids{ids[0], ids[1], ids[2]} {}

Face::~Face() {}

int Face::getID(int vert) const { return _ids[vert]; }

void Face::setID(int vert, int id) { _ids[vert] = id; }

int Face::vertexCount() const { return 3; }


//----- MESH

Mesh::Mesh() {}

Mesh::~Mesh() {}

void Mesh::reserveFaces(int count) {
    const auto maxCapacity = _faces.max_size();
    const auto newCapacity = min(count + _faceCount, maxCapacity);
    _faces.reserve(newCapacity);
}

u32 Mesh::getFaceCount() const { return _faceCount; }

Face &Mesh::getFace(int id) {
    if (id >= _faceCount)
        throw std::runtime_error("Mesh::getFace bad index");
    return _faces.at(id);
}

const Face &Mesh::getFace(int id) const {
    if (id >= _faceCount)
        throw std::runtime_error("Mesh::getFace bad index");
    return _faces.at(id);
}

void Mesh::addFace(const Face &face) {
    _faces.emplace_back(face);
    _faceCount++;
}

Face &Mesh::newFace() {
    _faces.emplace_back();
    _faceCount++;
    return _faces.back();
}

Face &Mesh::newFace(int *ids) {
    _faces.emplace_back(ids);
    _faceCount++;
    return _faces.back();
}

void Mesh::reserveVertices(int count) {
    const auto maxCapacity = _vertices.max_size();
    const auto newCapacity = min(count + _verticesCount, maxCapacity);
    _vertices.reserve(newCapacity);
}

u32 Mesh::getVerticesCount() const { return _verticesCount; }

const Vertex &Mesh::getVertex(int id) const {
    if (id >= _verticesCount)
        throw std::runtime_error("Mesh::getVertex bad index");
    return _vertices.at(id);
}

Vertex &Mesh::getVertex(int id) {
    if (id >= _verticesCount)
        throw std::runtime_error("Mesh::getVertex bad index");
    return _vertices.at(id);
}

void Mesh::addVertex(const Vertex &vert) {
    _vertices.emplace_back(vert);
    _verticesCount++;
}

Vertex &Mesh::newVertex() {
    _vertices.emplace_back();
    _verticesCount++;
    return _vertices.back();
}

Vertex &Mesh::newVertex(const world::vec3d &position,
                        const world::vec3d &normal,
                        const world::vec2d &texture) {
    _vertices.emplace_back(position, normal, texture);
    _verticesCount++;
    return _vertices.back();
}
} // namespace world
