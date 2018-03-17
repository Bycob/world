#pragma once

#include <worldapi/worldapidef.h>

#include "World.h"
#include "IWorldDecorator.h"
#include "../terrain/Ground.h"

namespace world {
	class FlatWorld;

	typedef IWorldDecorator<FlatWorld> IFlatWorldDecorator;

	class PrivateFlatWorld;

	class WORLDAPI_EXPORT FlatWorld : public World {
	public:
		FlatWorld();

		~FlatWorld() override;

		void addFlatWorldDecorator(IFlatWorldDecorator *decorator);

		Ground &ground();

	protected:
		void onFirstExploration(WorldZone &chunk) override;

	private:
		PrivateFlatWorld *_internal;

		Ground _ground;
	};
}
