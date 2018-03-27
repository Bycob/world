#pragma once

#include <worldapi/worldapidef.h>

namespace world {
	class WorldZone;

	template<typename T>
	class IWorldDecorator {
	public:
		virtual ~IWorldDecorator() = default;

		virtual void decorate(T &world, WorldZone &zone) = 0;
	};
}