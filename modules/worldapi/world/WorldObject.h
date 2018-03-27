#pragma once

#include <worldapi/worldapidef.h>

#include "../maths/MathsHelper.h"
#include "IChunkSystem.h"
#include "ICollector.h"

namespace world {

	class WORLDAPI_EXPORT WorldObject {
	public:
		WorldObject();

		virtual ~WorldObject();

		void setPosition3D(const vec3d &pos);

		vec3d getPosition3D() const { return _position; }

		virtual void collectWholeObject(ICollector &collector);

	protected:
		vec3d _position;
	};
}