//
// Created by louis on 20/04/17.
//

#ifndef WORLD_WORLDNODE_H
#define WORLD_WORLDNODE_H

#include <worldapi/worldapidef.h>

#include <string>
#include <set>

#define INIT_TYPE(NAME, ...) WorldNodeType NAME = WorldNodeType::create(__VA_ARGS__);

class WorldNodeType {
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

class WorldNode {
public:
    WorldNode(const WorldNodeType & type);
    virtual ~WorldNode();

private:
    const WorldNodeType _nodeType;
};


#endif //WORLD_WORLDNODE_H
