#pragma once

#include "core/WorldConfig.h"

#include "math/Vector.h"
#include "math/MathsHelper.h"
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