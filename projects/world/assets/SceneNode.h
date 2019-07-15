#pragma once

#include "world/core/WorldConfig.h"

#include <memory>

#include "world/core/Memory.h"
#include "world/math/MathsHelper.h"
#include "Mesh.h"

namespace world {

class WORLDAPI_EXPORT SceneNode {
public:
    SceneNode();

    SceneNode(std::string meshID);

    void setMesh(std::string meshID) { _meshID = meshID; }

    std::string getMeshID() const { return _meshID; }

    void setMaterialID(std::string materialID) { _materialID = materialID; }

    std::string getMaterialID() const { return _materialID; }

    void setPosition(vec3d position) { _position = position; }

    const vec3d &getPosition() const { return _position; }

    void setScale(vec3d scale) { _scale = scale; }

    vec3d getScale() const { return _scale; }

    void setRotation(vec3d rotation) { _rotation = rotation; }

    vec3d getRotation() const { return _rotation; }

private:
    std::string _meshID;
    std::string _materialID;

    vec3d _position;
    vec3d _rotation;
    vec3d _scale;
};
} // namespace world
