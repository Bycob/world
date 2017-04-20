//
// Created by louis on 20/04/17.
//

#ifndef WORLD_WORLDNODE_H
#define WORLD_WORLDNODE_H

#include <string>

class WorldNodeType {
public:
    WorldNodeType(const std::string & name);
    WorldNodeType(const WorldNodeType & other);

    const std::string & name();
private:
    const std::string _name;
};

class WorldNode {
public:
    WorldNode(const WorldNodeType & type);
    virtual ~WorldNode();

private:
    const WorldNodeType _nodeType;
};


#endif //WORLD_WORLDNODE_H
