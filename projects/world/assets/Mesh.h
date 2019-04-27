#pragma once

#include "world/core/WorldConfig.h"

#include <string>
#include <vector>
#include <stdexcept>

#include "world/math/Vector.h"
#include "world/core/WorldTypes.h"

namespace world {

class Vertex {
public:
    Vertex() : _position{}, _normal{}, _texture{} {}

    Vertex(const vec3d &position, const vec3d &normal, const vec2d &texture)
            : _position(position), _normal(normal), _texture(texture) {}

    vec3d getPosition() const { return _position; }

    vec3d getNormal() const { return _normal; }

    vec2d getTexture() const { return _texture; }

    void setPosition(const vec3d &position) { _position = position; }

    void setPosition(double x, double y, double z) { _position = {x, y, z}; }

    void setNormal(const vec3d &normal) { _normal = normal; }

    void setNormal(double x, double y, double z) { _normal = {x, y, z}; }

    void setTexture(const vec2d &texture) { _texture = texture; }

    void setTexture(double x, double y) { _texture = {x, y}; }

private:
    vec3d _position;
    vec3d _normal;
    vec2d _texture;
};


class WORLDAPI_EXPORT Face {

public:
    Face();

    Face(int ids[3]);

    virtual ~Face();

    void setID(int vert, int id);

    int getID(int vert) const;

    int vertexCount() const;

private:
    int _ids[3];
};


class WORLDAPI_EXPORT Mesh {

public:
    Mesh();

    virtual ~Mesh();

    /** Tells the mesh that we are going to add a certain amount
     * of faces. This method enables the mesh to adapt its buffer
     * for the desired amount, and thus to improve performances.
     * @param capacity The amount of faces we want to add. */
    void reserveFaces(int capacity);

    u32 getFaceCount() const;

    Face &getFace(u32 id);

    const Face &getFace(u32 id) const;

    void addFace(const Face &face);

    Face &newFace();

    Face &newFace(int ids[3]);

    void clearFaces();

    /** Tells the mesh that we are going to add a certain amount
     * of vertices. This method enables the mesh to adapt its buffer
     * for the desired amount, and thus to improve performances.
     * @param capacity The amount of vertices we want to add. */
    void reserveVertices(u32 capacity);

    u32 getVerticesCount() const;

    Vertex &getVertex(u32 id);

    const Vertex &getVertex(u32 id) const;

    void addVertex(const Vertex &vert);

    Vertex &newVertex();

    Vertex &newVertex(const vec3d &position, const vec3d &normal = {0, 0, 1},
                      const vec2d &texture = {0, 0});

    void clearVertices();

private:
    u32 _verticesCount = 0;
    std::vector<Vertex> _vertices;
    u32 _faceCount = 0;
    std::vector<Face> _faces;
};
} // namespace world
