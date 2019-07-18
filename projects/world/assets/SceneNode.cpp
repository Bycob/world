#include "SceneNode.h"

namespace world {

SceneNode::SceneNode() : SceneNode("", "") {}

SceneNode::SceneNode(std::string meshID, std::string materialID)
        : _meshID(meshID), _materialID(materialID), _position(0, 0, 0),
          _scale(1, 1, 1) {}

SceneNode::SceneNode(const Mesh &mesh) : SceneNode(mesh.getName()) {}

SceneNode::SceneNode(const Mesh &mesh, const Material &material)
        : SceneNode(mesh.getName(), material.getName()) {}

} // namespace world
