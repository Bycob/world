//
// Created by louis on 20/04/17.
//

#include "WorldObject.h"

#include <stdexcept>
#include <iostream>

using namespace maths;

ObjectPosition::ObjectPosition(const vec3d & pos, int lodMax)
	: _pos(pos), _lodMax(lodMax) {

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
	_position = ObjectPosition(pos, _position.getMaxLOD());
}

const ObjectPosition & WorldObject::getPosition() const {
	return _position;
}

void WorldObject::fillScene(Scene & scene) const {

}