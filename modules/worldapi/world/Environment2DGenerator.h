#pragma once

#include <worldapi/worldapidef.h>

#include "MapGenerator.h"
#include "FlatWorldGenerator.h"
#include "../terrain/TerrainGenerator.h"

class WORLDAPI_EXPORT Environment2DGenerator : public FlatWorldGenNode, public IFlatWorldExpander {
public:
	Environment2DGenerator();
	Environment2DGenerator(TerrainGenerator * terrainGenerator, MapGenerator * mapGenerator);
	Environment2DGenerator(const Environment2DGenerator & other);
	virtual ~Environment2DGenerator();

	void setTerrainGenerator(TerrainGenerator * generator);
	void setMapGenerator(MapGenerator * generator);

	virtual void generate(FlatWorld & world);
	virtual void onAddExpander(FlatWorld & world);
	virtual void expand(FlatWorld & world, const IPointOfView & from);
private:
	static const Environment2DMetadata DEFAULT_METADATA;

	Environment2DMetadata _metadata;

	std::unique_ptr<TerrainGenerator> _terrainGenerator;
	std::unique_ptr<MapGenerator> _mapGenerator;

	void applyMap(TerrainTile & tile, const Map & map, bool unapply = false);
};

