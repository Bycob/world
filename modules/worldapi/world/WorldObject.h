#pragma once

#include <worldapi/worldapidef.h>

#include "../maths/MathsHelper.h"
#include "ChunkSystem.h"

namespace world {

	class CollectorObject;

	class WORLDAPI_EXPORT WorldObject {
	public:
		WorldObject();

		virtual ~WorldObject();

		void setPosition3D(const vec3d &pos);

		vec3d getPosition3D() const { return _position; }

		virtual void collectWholeObject(WorldZone &zone, CollectorObject &collector);

	protected:
		vec3d _position;
	};
}