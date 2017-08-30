#pragma once

#include <tuple>
#include <memory>

#include <worldapi/maths/mathshelper.h>
#include "WorldObject.h"

class ChunkPosition {
public:
	ChunkPosition(int x = 0, int y = 0, int z = 0, int lod = 0);
	ChunkPosition(const ChunkPosition & other);
	~ChunkPosition();

	const maths::vec3i & getPosition3D() const;

	bool operator<(const ChunkPosition & other) const;
private:
	maths::vec3i _pos;
	int _lod;
};

class Chunk {
public:
	Chunk(const ChunkPosition & position);
	virtual ~Chunk();

	void addObject(WorldObject * object);
	
private:
	ChunkPosition _position;
	std::vector<std::unique_ptr<WorldObject>> _objects;
};

