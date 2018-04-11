#pragma once

#include "core/WorldConfig.h"

#include "core/World.h"
#include "IGround.h"

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

		IGround &ground();

		void addFlatWorldDecorator(FlatWorldDecorator *decorator);

		void collect(const WorldZone &zone, ICollector &collector) override;

		void collect(const WorldZone &zone, FlatWorldCollector &collector);

	protected:
		void onFirstExploration(WorldZone &chunk) override;

	private:
		PrivateFlatWorld *_internal;

		std::unique_ptr<IGround> _ground;
	};
}
