#pragma once

#include <worldapi/worldapidef.h>

#include <memory>
#include <vector>
#include <string>
#include <set>

#define DECL_TYPE static WorldNodeType type;
#define INIT_TYPE(CLASSNAME, ...) WorldNodeType CLASSNAME::type = WorldNodeType::create(__VA_ARGS__);
#define TYPE(CLASSNAME) CLASSNAME::type

class WORLDAPI_EXPORT WorldNodeType {
public:
	static const WorldNodeType & create(const std::string & name, bool unique = false);

	WorldNodeType(const WorldNodeType & other);

	const std::string & name() const;
	bool unique() const;

	bool operator==(const WorldNodeType & type) const;
	bool operator<(const WorldNodeType & type) const;
private:
	WorldNodeType(const std::string & name, bool unique);

	std::string _name;
	bool _unique;
};

class World;

class WORLDAPI_EXPORT WorldNode {
public:
	WorldNode(const World * world, const WorldNodeType & type);
	virtual ~WorldNode();

	const WorldNodeType & type() const;
private:
	const WorldNodeType _nodeType;
	const World * _world;
};

class InternalWorldGenerator;
class WorldGenerator;

class WORLDAPI_EXPORT World {
public:
	World();
	World(const WorldGenerator & generator);
	virtual ~World();

	template <typename T> T & createNode() {
		if (checkNodeTypeInternal(T::type)) {
			T * newNode = new T(*this);
			_nodes.emplace_back(newNode);
			return *newNode;
		}
		else {
			throw std::runtime_error("The node can not be created");
		}
	};

	WorldGenerator & getGenerator();

	// getAssets(zone, level detail)

	// getAssets(vec3d from, level detail scale)
private:
	InternalWorldGenerator * _internal;

    std::vector<std::unique_ptr<WorldNode>> _nodes;

    friend class WorldGenerator;

	bool checkNodeTypeInternal(const WorldNodeType & type) const;
};

