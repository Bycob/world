#pragma once

#include <worldapi/worldapidef.h>

#include <memory>
#include <vector>
#include <string>
#include <set>
#include <functional>

#include "../WorldFolder.h"
#include "../Scene.h"
#include "Chunk.h"
#include "IExpander.h"
#include "IChunkDecorator.h"
#include "LODData.h"

class World;

typedef IExpander<World> IWorldExpander;
typedef IChunkDecorator<World> IWorldChunkDecorator;

class PrivateWorld;

class WORLDAPI_EXPORT World {
public:
	World();
	World(const World & world) = delete;
	virtual ~World();

	void setLODData(int lod, const LODData & data);
	LODData & getLODData(int lod) const;

	void addObject(WorldObject * object);

	bool isChunkGenerated(const ChunkPosition & position);
	Chunk & getChunk(const ChunkPosition & position) const;
	ChunkPosition getChunkPosition(const ObjectPosition & position);

	void addExpander(IWorldExpander * expander);
	void addChunkDecorator(IWorldChunkDecorator * decorator);

	virtual void expand(const IPointOfView & from);
	virtual void generateChunk(const ChunkPosition & position);
	
	virtual Scene * createSceneFrom(const IPointOfView & from) const;

	// getAssets(zone, level detail)
	// getAssets(vec3d from, level detail scale)
protected:
	LODData & getOrCreateLODData(int lod);
	Chunk & getOrCreateChunk(const ChunkPosition & position);

	void iterateChunkPosInSight(const IPointOfView & from, const LODData & data, const std::function<void(const ChunkPosition&)> & action) const;

	virtual void callExpanders(const IPointOfView & from);
	virtual void generateChunk(Chunk & chunk);

private:
	PrivateWorld * _internal;

	WorldFolder _directory; // TODO remplacer ça par un ICache, qui peut être un dossier, une interface réseau, rien...
};

