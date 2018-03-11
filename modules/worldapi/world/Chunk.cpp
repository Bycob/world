#include "Chunk.h"

#include "WorldObject.h"

using namespace maths;

class PrivateChunk {
public:
	std::vector<std::unique_ptr<WorldObject>> _objects;
};

Chunk::Chunk(const vec3d& position, const vec3d & size)
	: _internal(new PrivateChunk()),
	  _offset(position),
	  _size(size) {

}

Chunk::~Chunk() {
	delete _internal;
}

void Chunk::setDetailSizeBounds(double min, double max) {
	_minDetailSize = min;
	_maxDetailSize = max;
}

vec3d Chunk::toAbsolutePosition(const vec3d & relative) const {
	return _size * _offset;
}

vec3d Chunk::toRelativePosition(const vec3d & absolute) const {
	return absolute - _offset;
}

void Chunk::addObject(WorldObject * object) {
	_internal->_objects.emplace_back(object);
}

void Chunk::addObjectInternal(WorldObject * object) {
	_internal->_objects.emplace_back(object);
}