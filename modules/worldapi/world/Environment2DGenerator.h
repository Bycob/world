#pragma once

#include <worldapi/worldapidef.h>

#include "MapGenerator.h"
#include "FlatWorldGenerator.h"
#include "IFlatWorldExpander.h"
#include "../terrain/TerrainGenerator.h"

class WORLDAPI_EXPORT Environment2DGenerator : public FlatWorldGenNode, IFlatWorldExpander {
public:
	Environment2DGenerator();
	Environment2DGenerator(TerrainGenerator * terrainGenerator, MapGenerator * mapGenerator);
	Environment2DGenerator(const Environment2DGenerator & other);
	virtual ~Environment2DGenerator();

	virtual void generate(FlatWorld & world);
	virtual void expand(FlatWorld & world, const IPointOfView & from);

	void setTerrainGenerator(TerrainGenerator * generator);
	void setMapGenerator(MapGenerator * generator);
private:
	std::unique_ptr<TerrainGenerator> _terrainGenerator;
	std::unique_ptr<MapGenerator> _mapGenerator;

	void applyMap(TerrainTile & tile, const Map & map, bool unapply = false);
};

