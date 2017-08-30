#include "Chunk.h"

using namespace maths;

ChunkPosition::ChunkPosition(int x, int y, int z, int lod)
	: _pos(x, y, z), _lod(lod) {

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



Chunk::Chunk(const ChunkPosition & position) {

}

Chunk::~Chunk() {

}