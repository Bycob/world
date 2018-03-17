#pragma once

#include <worldapi/worldapidef.h>

#include <tuple>
#include <memory>
#include <iterator>
#include <functional>

#include <worldapi/maths/MathsHelper.h>

namespace world {
	class WorldObject;

	class ChunkIterator;

	class PrivateChunk;

	class WORLDAPI_EXPORT Chunk {
	public:
		Chunk(const vec3d &offset, const vec3d &size);

		Chunk(const Chunk &chunk) = delete;

		virtual ~Chunk();

		void setDetailSizeBounds(double min, double max);

		double getMinDetailSize() const { return _minDetailSize; }

		double getMaxDetailSize() const { return _maxDetailSize; }

		const vec3d &getSize() const { return _size; }

		const vec3d &getOffset() const { return _offset; }

		void addObject(WorldObject *object);

		template<typename T, typename... Args>
		T &createObject(Args... args) {
			addObjectInternal(new T(args...));
		}

		void forEachObject(const std::function<void(WorldObject &)> &action);

	private:
		double _minDetailSize = 0;
		double _maxDetailSize = 1e100;
		vec3d _offset;
		vec3d _size;

		PrivateChunk *_internal;

		void addObjectInternal(WorldObject *object);
	};
}
