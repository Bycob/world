#pragma once

#include <worldapi/worldapidef.h>

#include <memory>
#include <vector>
#include <string>
#include <set>
#include <functional>

#include "../WorldFolder.h"
#include "Chunk.h"
#include "WorldObject.h"
#include "LODData.h"
#include "IWorldDecorator.h"

class World;

typedef IWorldDecorator<World> IWorldChunkDecorator;

class PrivateWorld;

class WORLDAPI_EXPORT World {
public:
	/** Crée un monde complet qui peut être utilisé pour appréhender 
	les capacité de la bibliothèque 'World'. */
	static World * createDemoWorld();

	World();
	World(const World & world) = delete;
	virtual ~World();

	void setLODData(int lod, const LODData & data);
	LODData & getLODData(int lod) const;

	void addObject(WorldObject * object);

	Chunk & getChunk(const ChunkID & position);
	ChunkID getChunkPosition(const ObjectPosition & position);

	void addChunkDecorator(IWorldChunkDecorator * decorator);

	// getAssets(zone, level detail)
	// getAssets(vec3d from, level detail scale)
protected:
	LODData & getOrCreateLODData(int lod);
	Chunk & getOrCreateChunk(const ChunkID & position);

	bool isChunkGenerated(const ChunkID & position) const;
	virtual void generateChunk(Chunk & chunk);

private:
	PrivateWorld * _internal;

	WorldFolder _directory; // TODO remplacer ça par un ICache, qui peut être un dossier, une interface réseau, rien...
};

