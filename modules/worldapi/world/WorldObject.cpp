//
// Created by louis on 20/04/17.
//

#include "WorldObject.h"

#include <stdexcept>
#include <iostream>

using namespace maths;

ObjectPosition::ObjectPosition(const vec3d & offset)
	: _pos(offset) {

}

ObjectPosition::~ObjectPosition() {

}

const maths::vec3d & ObjectPosition::getPosition3D() const {
	return _pos;
}


WorldObject::WorldObject() {

}

WorldObject::~WorldObject() {

}

void WorldObject::setPosition3D(const maths::vec3d & pos) {
	_position = ObjectPosition(pos);
}

const ObjectPosition & WorldObject::getPosition() const {
	return _position;
}