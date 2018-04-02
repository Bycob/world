#pragma once

#include "core/WorldConfig.h"

#include <memory>
#include <vector>
#include <string>
#include <set>
#include <functional>

#include "IChunkSystem.h"
#include "WorldObject.h"
#include "WorldFolder.h"
#include "IWorldDecorator.h"
#include "ICollector.h"

namespace world {

	class World;

	typedef IWorldDecorator<World> WorldDecorator;

	class PrivateWorld;

	class WORLDAPI_EXPORT World {
	public:
		/** Create a complete and rich world that can be used
         * as a demonstration of the API power ! */
		static World *createDemoWorld();

		World();

		World(const World &world) = delete;

		virtual ~World();

		void addWorldDecorator(WorldDecorator *decorator);

		// NAVIGATION
		WorldZone exploreNeighbour(const WorldZone &zone, const vec3d &direction);

		WorldZone exploreLocation(const vec3d &location);

		std::vector<WorldZone> exploreInside(const WorldZone &zone);

        // ASSETS
        virtual void collect(const WorldZone &zone, ICollector &collector);

	protected:
		virtual void onFirstExploration(WorldZone &chunk);

	private:
		PrivateWorld *_internal;

		std::unique_ptr<IChunkSystem> _chunkSystem;
		WorldFolder _directory; // TODO remplacer ça par un ICache, qui peut être un dossier, une interface réseau, rien...
	};
}
