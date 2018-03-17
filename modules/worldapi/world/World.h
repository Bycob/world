#pragma once

#include <worldapi/worldapidef.h>

#include <memory>
#include <vector>
#include <string>
#include <set>
#include <functional>

#include "../WorldFolder.h"
#include "ChunkSystem.h"
#include "WorldObject.h"
#include "LODData.h"
#include "IWorldDecorator.h"

namespace world {

	class World;

	typedef IWorldDecorator<World> IWorldChunkDecorator;

	class PrivateWorld;

	class WORLDAPI_EXPORT World {
	public:
		/** Create a complete and rich world that can be used
         * as a demonstration of the API power ! */
		static World *createDemoWorld();

		World();

		World(const World &world) = delete;

		virtual ~World();

		void addChunkDecorator(IWorldChunkDecorator *decorator);

		// NAVIGATION
		WorldZone exploreNeighbour(const WorldZone &zone, const vec3d &direction);

		WorldZone exploreLocation(const vec3d &location);

		std::vector<WorldZone> exploreInside(const WorldZone &zone);

	protected:
		virtual void onFirstExploration(WorldZone &chunk);

	private:
		PrivateWorld *_internal;

		ChunkSystem _chunkSystem;
		WorldFolder _directory; // TODO remplacer ça par un ICache, qui peut être un dossier, une interface réseau, rien...
	};
}
