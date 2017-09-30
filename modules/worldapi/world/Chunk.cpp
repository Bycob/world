#include "Chunk.h"

using namespace maths;

ChunkPosition::ChunkPosition(int x, int y, int z, int lod)
	: _pos(x, y, z), _lod(lod) {

}

ChunkPosition::ChunkPosition(const maths::vec3i & pos, int lod) 
	: _pos(pos), _lod(lod) {

}

ChunkPosition::ChunkPosition(const ChunkPosition & other)
	: _pos(other._pos), _lod(other._lod) {

}

ChunkPosition::~ChunkPosition() {

}

const vec3i & ChunkPosition::getPosition3D() const {
	return _pos;
}

bool ChunkPosition::operator<(const ChunkPosition & other) const {
	return _lod < other._lod ? true : (
		_pos.x < other._pos.x ? true : (
			_pos.y < other._pos.y ? true : (
				_pos.z < other._pos.z)));
}



class PrivateChunk {
public:
	std::vector<std::unique_ptr<WorldObject>> _objects;
};

Chunk::Chunk(const ChunkPosition & position, const vec3d & size) 
	: _internal(new PrivateChunk()), _position(position), _size(size) {

}

Chunk::~Chunk() {
	delete _internal;
}

vec3d Chunk::toAbsolutePosition(const vec3d & relative) const {
	return _size * _position.getPosition3D() + relative;
}

vec3d Chunk::toRelativePosition(const vec3d & absolute) const {
	return absolute - _size * _position.getPosition3D();
}

void Chunk::addObject(WorldObject * object) {
	_internal->_objects.emplace_back(object);
}

void Chunk::fillScene(Scene & scene) {
	for (auto & object : _internal->_objects) {
		object->fillScene(scene);
	}
}

void Chunk::addObjectInternal(WorldObject * object) {
	_internal->_objects.emplace_back(object);
}