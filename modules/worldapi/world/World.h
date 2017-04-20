#pragma once

#include <worldapi/worldapidef.h>

#include <memory>
#include <vector>

#include "WorldGenerator.h"
#include "../WorldNode.h"

class WORLDAPI_EXPORT World {
public:
	World();
	virtual ~World();



	// getAssets(zone, level detail)

	// getAssets(vec3d from, level detail scale)
private:
	std::unique_ptr<WorldGenerator> _gen;

    std::vector<WorldNode> _nodes;
};

