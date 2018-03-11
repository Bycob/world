//
// Created by louis on 20/04/17.
//

#include "WorldObject.h"

#include <stdexcept>
#include <iostream>

using namespace maths;

WorldObject::WorldObject() {

}

WorldObject::~WorldObject() {

}

void WorldObject::setPosition3D(const maths::vec3d & pos) {
	_position = pos;
}