#pragma once

#include <worldapi/worldapidef.h>

#include <memory>
#include <vector>
#include <string>
#include <set>

#include "Chunk.h"
#include "../WorldFolder.h"
#include "IWorldExpander.h"

class PrivateWorld;

class WORLDAPI_EXPORT World {
public:
	World();
	World(const World & world) = delete;
	virtual ~World();

	void addExpander(IWorldExpander * expander);
	Chunk & getChunk(const ChunkPosition & position);

	virtual void expand(const IPointOfView & from);

	// getAssets(zone, level detail)
	// getAssets(vec3d from, level detail scale)
private:
	PrivateWorld * _internal;

	WorldFolder _directory; // TODO remplacer ça par un ICache, qui peut être un dossier, une interface réseau, rien...
};

