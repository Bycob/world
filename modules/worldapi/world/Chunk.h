#pragma once

#include <worldapi/worldapidef.h>

#include <tuple>
#include <memory>

#include <worldapi/maths/MathsHelper.h>
#include "WorldObject.h"
#include "../Scene.h"

class WORLDAPI_EXPORT ChunkPosition {
public:
	ChunkPosition(int x = 0, int y = 0, int z = 0, int lod = 0);
	ChunkPosition(const maths::vec3i & pos, int lod = 0);
	ChunkPosition(const ChunkPosition & other);
	~ChunkPosition();

	const maths::vec3i & getPosition3D() const;
	int getLOD() const { return _lod; }

	bool operator<(const ChunkPosition & other) const;
private:
	maths::vec3i _pos;
	int _lod;
};

class PrivateChunk;

class WORLDAPI_EXPORT Chunk {
public:
	Chunk(const ChunkPosition & position, const maths::vec3d & size);
	virtual ~Chunk();

	const ChunkPosition & getChunkPosition() const { return _position; };
	const maths::vec3d & getSize() const { return _size; }
	
	maths::vec3d toAbsolutePosition(const maths::vec3d & relative) const;
	maths::vec3d toRelativePosition(const maths::vec3d & absolute) const;

	void addObject(WorldObject * object);
	
	template <typename T, typename... Args>
	T & createObject(Args... args) {
		addObjectInternal(new T(args...));
	}

	void fillScene(Scene & scene);

private:
	ChunkPosition _position;
	maths::vec3d _size;

	PrivateChunk * _internal;

	void addObjectInternal(WorldObject * object);
};

