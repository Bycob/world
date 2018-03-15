#pragma once

#include <worldapi/worldapidef.h>

#include <tuple>
#include <memory>
#include <iterator>

#include <worldapi/maths/MathsHelper.h>

class WorldObject;

class ChunkIterator;

class PrivateChunk;

class WORLDAPI_EXPORT Chunk {
public:
	Chunk(const maths::vec3d& offset, const maths::vec3d & size);
	Chunk(const Chunk &chunk) = delete;
	virtual ~Chunk();

	void setDetailSizeBounds(double min, double max);
	double getMinDetailSize() const { return _minDetailSize; }
	double getMaxDetailSize() const { return _maxDetailSize; }

	const maths::vec3d & getSize() const { return _size; }
    const maths::vec3d & getOffset() const { return _offset; }

	void addObject(WorldObject * object);
	
	template <typename T, typename... Args>
	T & createObject(Args... args) {
		addObjectInternal(new T(args...));
	}

	void forEachObject(const std::function<void(WorldObject&)> & action);
private:
	double _minDetailSize = 0;
	double _maxDetailSize = 1e100;
	maths::vec3d _offset;
	maths::vec3d _size;

	PrivateChunk * _internal;

	void addObjectInternal(WorldObject * object);
};
