#pragma once

#include <worldapi/worldapidef.h>

#include <tuple>
#include <memory>

#include <worldapi/maths/MathsHelper.h>

class WorldObject;

class WORLDAPI_EXPORT ChunkID {
public:
	ChunkID(int x = 0, int y = 0, int z = 0, int lod = 0);
	ChunkID(const maths::vec3i & pos, int lod = 0);
	ChunkID(const ChunkID & other);
	~ChunkID();

	const maths::vec3i & getPosition3D() const;
	int getLOD() const { return _lod; }

	bool operator<(const ChunkID & other) const;
private:
	maths::vec3i _pos;
	int _lod;
};

class PrivateChunk;

class WORLDAPI_EXPORT Chunk {
public:
	Chunk(const ChunkID & position, const maths::vec3d & size);
	virtual ~Chunk();

	const ChunkID & getChunkPosition() const { return _position; }
	const maths::vec3d & getSize() const { return _size; }
    const maths::vec3d & getOffset() const { return _offset; }
	
	maths::vec3d toAbsolutePosition(const maths::vec3d & relative) const;
	maths::vec3d toRelativePosition(const maths::vec3d & absolute) const;

	void addObject(WorldObject * object);
	
	template <typename T, typename... Args>
	T & createObject(Args... args) {
		addObjectInternal(new T(args...));
	}

private:
	ChunkID _position;
	maths::vec3d _offset;
	maths::vec3d _size;

	PrivateChunk * _internal;

	void addObjectInternal(WorldObject * object);
};

