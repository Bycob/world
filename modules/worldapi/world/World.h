#pragma once

#include <worldapi/worldapidef.h>

#include <memory>
#include <vector>
#include <string>
#include <set>

#include "../WorldFolder.h"

#define DECL_TYPE static const WorldNodeType & type();
#define INIT_TYPE(CLASSNAME, ...) \
const WorldNodeType & CLASSNAME::type() { \
	static WorldNodeType type = WorldNodeType::create(__VA_ARGS__); \
	return type; \
}

#define TYPE(CLASSNAME) CLASSNAME::type()

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

	void setDirectory(const std::string & directory);
	const WorldFolder & getDirectory() const;
protected:
	const WorldNodeType _nodeType;
	const World * _world;

	WorldFolder _directory;
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
	template <typename T> const T & getUniqueNode() const;

	WorldGenerator & getGenerator();

	// getAssets(zone, level detail)
	// getAssets(vec3d from, level detail scale)
private:
	PrivateWorld * _internal;

	WorldFolder _directory;

    friend class WorldGenerator;

	// Retourne une référence vers le champ _internal->_nodes
	std::vector<std::unique_ptr<WorldNode>> & _nodes();
	bool checkNodeTypeInternal(const WorldNodeType & type) const;
};

#include "World.inl"
