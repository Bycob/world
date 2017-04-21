//
// Created by louis on 20/04/17.
//

#ifndef WORLD_WORLDNODE_H
#define WORLD_WORLDNODE_H

#include <worldapi/worldapidef.h>

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


#endif //WORLD_WORLDNODE_H
