#pragma once

#include <worldapi/worldapidef.h>

#include <tuple>
#include <memory>

#include <worldapi/maths/MathsHelper.h>

class WorldObject;
class PrivateChunk;

class WORLDAPI_EXPORT Chunk {
public:
	Chunk(const maths::vec3d& offset, const maths::vec3d & size);
	virtual ~Chunk();

	void setDetailSizeBounds(double min, double max);
	double getMinDetailSize() const { return _minDetailSize; }
	double getMaxDetailSize() const { return _maxDetailSize; }

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
	double _minDetailSize = 0;
	double _maxDetailSize = 1e100;
	maths::vec3d _offset;
	maths::vec3d _size;

	PrivateChunk * _internal;

	void addObjectInternal(WorldObject * object);
};

