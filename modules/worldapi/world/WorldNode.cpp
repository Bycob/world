//
// Created by louis on 20/04/17.
//

#include "WorldNode.h"

#include <stdexcept>
#include <iostream>

#include "World.h"

const WorldNodeType& WorldNodeType::create(const std::string &name, bool unique) {
    static std::set<WorldNodeType> _types;
    auto created = _types.insert(WorldNodeType(name, unique));

    if (!created.second) {
        std::cerr << "Initialization error : Node type \"" + name + "\" is defined at least twice." << std::endl;
        exit(1);
    }
    return *created.first;
}

WorldNodeType::WorldNodeType(const std::string &name, bool unique)
        : _name(name), _unique(unique) {

}

WorldNodeType::WorldNodeType(const WorldNodeType &other) : _name(other._name) {

}

const std::string& WorldNodeType::name() const {
    return _name;
}

bool WorldNodeType::unique() const {
    return _unique;
}

bool WorldNodeType::operator==(const WorldNodeType &type) const {
    return type._name == this->_name;
}

bool WorldNodeType::operator<(const WorldNodeType &type) const {
    return this->_name < type._name;
}


WorldNode::WorldNode(const World * world, const WorldNodeType &type)
        : _nodeType(type), _world(world) {

}

WorldNode::~WorldNode() {

}

const WorldNodeType& WorldNode::type() const {
    return _nodeType;
}