//
// Created by louis on 30/04/17.
//

#ifndef WORLD_TERRAINSCENENODE_H
#define WORLD_TERRAINSCENENODE_H

#include <vector>

#include <irrlicht.h>

#include <worldapi/terrain/terrain.h>
#include <worldapi/world/Ground.h>
#include <worldapi/world/World.h>

class GroundSceneNode {
public:
    GroundSceneNode(irr::IrrlichtDevice * device);

    void initialize(const World &world);
private:
    irr::scene::ISceneManager * _sceneManager;
    irr::io::IFileSystem * _fileSystem;

	std::vector<irr::scene::ITerrainSceneNode*> _terrainNodes;

    void addNode(const TerrainTile & tile, const Ground & groundModule);
};


#endif //WORLD_TERRAINSCENENODE_H
