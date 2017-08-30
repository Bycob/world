#pragma once

#include <worldapi/worldapidef.h>

#include <map>

#include "../terrain/Ground.h"
#include "Map.h"

class PrivateEnvironment2D;

class Environment2D {
public:
	Environment2D();
	virtual ~Environment2D();

	Map & getMap() { return *_map; }
	const Map & getMap() const { return *_map; }
	void setMap(Map * map);

	Ground & getGround() { return *_ground; }
	const Ground & getGround() const { return *_ground; }
	void setGround(Ground * ground);

private:
	std::unique_ptr<Ground> _ground;
	std::unique_ptr<Map> _map;
};

