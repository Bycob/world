#include "WorldObject.h"

#include <stdexcept>
#include <iostream>

#include "LODGridChunkSystem.h"
#include "Collector.h"

namespace world {

WorldObject::WorldObject() {}

WorldObject::~WorldObject() {}

void WorldObject::setPosition3D(const vec3d &pos) { _position = pos; }

void WorldObject::collectWholeObject(ICollector &collector) {}
} // namespace world
