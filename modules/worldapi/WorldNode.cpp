//
// Created by louis on 20/04/17.
//

#include "WorldNode.h"

WorldNodeType::WorldNodeType(const std::string &name) : _name(name) {

}

WorldNodeType::WorldNodeType(const WorldNodeType &other) : _name(other._name) {

}

const std::string& WorldNodeType::name() {
    return _name;
}


WorldNode::WorldNode(const WorldNodeType &type) : _nodeType(type) {

}

WorldNode::~WorldNode() {

}