#include "Chunk.h"

#include "WorldObject.h"

using namespace maths;

ChunkID::ChunkID(int x, int y, int z, int lod)
	: _pos(x, y, z), _lod(lod) {

}

ChunkID::ChunkID(const maths::vec3i & pos, int lod)
	: _pos(pos), _lod(lod) {

}

ChunkID::ChunkID(const ChunkID & other)
	: _pos(other._pos), _lod(other._lod) {

}

ChunkID::~ChunkID() {

}

const vec3i & ChunkID::getPosition3D() const {
	return _pos;
}

bool ChunkID::operator<(const ChunkID & other) const {
	return _lod < other._lod ? true : (
		_pos.x < other._pos.x ? true : (
			_pos.y < other._pos.y ? true : (
				_pos.z < other._pos.z)));
}



class PrivateChunk {
public:
	std::vector<std::unique_ptr<WorldObject>> _objects;
};

Chunk::Chunk(const ChunkID & position, const vec3d & size)
	: _internal(new PrivateChunk()), _position(position),
	  _offset(_position.getPosition3D() * size),
	  _size(size) {

}

Chunk::~Chunk() {
	delete _internal;
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