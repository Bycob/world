#pragma once

#include <worldapi/worldapidef.h>

#include "World.h"
#include "IWorldDecorator.h"
#include "../terrain/Ground.h"

namespace world {
	class FlatWorld;

	typedef IWorldDecorator<FlatWorld> FlatWorldDecorator;

	class PrivateFlatWorld;

	class WORLDAPI_EXPORT FlatWorld : public World {
	public:
		/** Create a complete and rich world that can be used
         * as a demonstration of the API power ! */
		static FlatWorld *createDemoFlatWorld();

		FlatWorld();

		~FlatWorld() override;

		Ground &ground();

		void addFlatWorldDecorator(FlatWorldDecorator *decorator);

		void collect(const WorldZone &zone, FlatWorldCollector &collector);

	protected:
		void onFirstExploration(WorldZone &chunk) override;

	private:
		PrivateFlatWorld *_internal;

		Ground _ground;
	};
}
