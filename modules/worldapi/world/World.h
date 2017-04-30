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

class PrivateWorld;
class WorldGenerator;

class WORLDAPI_EXPORT World {
public:
	World();
	World(const WorldGenerator & generator);
	World(const World & world) = delete;
	virtual ~World();

	// TODO renforcer l'architecture en templates pour que ça plante à la compilation et pas à l'execution
	template <typename T> T & createNode();
	template <typename T> T & getUniqueNode();

	WorldGenerator & getGenerator();

	// getAssets(zone, level detail)
	// getAssets(vec3d from, level detail scale)
private:
	PrivateWorld * _internal;

    friend class WorldGenerator;

	// Retourne une référence vers le champ _internal->_nodes
	std::vector<std::unique_ptr<WorldNode>> & _nodes();
	bool checkNodeTypeInternal(const WorldNodeType & type) const;
};

#include "World.inl"
