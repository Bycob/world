#include "WorldObject.h"

#include <stdexcept>
#include <iostream>

#include "ChunkSystem.h"
#include "WorldCollector.h"

using namespace maths;

WorldObject::WorldObject() {

}

WorldObject::~WorldObject() {

}

void WorldObject::setPosition3D(const maths::vec3d & pos) {
	_position = pos;
}

void WorldObject::collectWholeObject(WorldZone &zone, CollectorObject &collector) {

}