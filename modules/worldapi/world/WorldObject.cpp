#include "WorldObject.h"

#include <stdexcept>
#include <iostream>

#include "ChunkSystem.h"
#include "Collector.h"

namespace world {

	WorldObject::WorldObject() {

	}

	WorldObject::~WorldObject() {

	}

	void WorldObject::setPosition3D(const vec3d &pos) {
		_position = pos;
	}

	void WorldObject::collectWholeObject(WorldZone &zone, CollectorObject &collector) {

	}
}
