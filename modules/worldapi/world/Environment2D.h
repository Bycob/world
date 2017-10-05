#pragma once

#include <worldapi/worldapidef.h>

#include <map>

#include "../terrain/Ground.h"
#include "Map.h"

class PrivateEnvironment2D;

struct WORLDAPI_EXPORT Environment2DMetadata {
	float unitsPerMapPixel;
	float unitsPerTerrain;
};

class WORLDAPI_EXPORT Environment2D {
public:
	Environment2D();
	virtual ~Environment2D();

	Map & map() { return *_map; }
	const Map & getMap() const { return *_map; }
	void setMap(Map * map);

	Ground & ground() { return *_ground; }
	const Ground & getGround() const { return *_ground; }
	void setGround(Ground * ground);

	Environment2DMetadata & metadata() { return _metadata; }
	const Environment2DMetadata & getMetadata() const { return _metadata; }
	void setMetadata(const Environment2DMetadata & metadata);
private:
	Environment2DMetadata _metadata;

	std::unique_ptr<Ground> _ground;
	std::unique_ptr<Map> _map;

	friend class Environment2DGenerator;
};

