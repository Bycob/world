#pragma once

#include <worldapi/worldapidef.h>

#include <memory>
#include <vector>

#include "WorldGenerator.h"
#include "WorldNode.h"

class WORLDAPI_EXPORT World {
public:
	World();
	virtual ~World();

	template <typename T> T & createNode() {
		if (checkNodeTypeInternal(T::type)) {
			T * newNode = new T(*this);
			_nodes.emplace_back(newNode);
			return *newNode;
		}
		else {
			throw std::runtime_error("The node can't be created");
		}
	};

	// getAssets(zone, level detail)

	// getAssets(vec3d from, level detail scale)
private:
	std::unique_ptr<WorldGenerator> _gen;

    std::vector<std::unique_ptr<WorldNode>> _nodes;

    friend class WorldGenerator;

	bool checkNodeTypeInternal(const WorldNodeType & type) const;
};

