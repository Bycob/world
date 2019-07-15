#include "SceneNode.h"

namespace world {

SceneNode::SceneNode() : SceneNode("") {}

SceneNode::SceneNode(std::string meshID)
        : _meshID(meshID), _materialID(""), _position(0, 0, 0),
          _scale(1, 1, 1) {}
} // namespace world
