//
// Created by louis on 30/04/17.
//

#ifndef WORLD_TERRAINSCENENODE_H
#define WORLD_TERRAINSCENENODE_H

#include <irrlicht.h>

#include <worldapi/terrain/terrain.h>
#include <worldapi/world/World.h>

class GroundSceneNode {
public:
    GroundSceneNode(irr::IrrlichtDevice * device);

    void initialize(const World &world);
private:
    irr::scene::ISceneManager * _sceneManager;
    irr::io::IFileSystem * _fileSystem;

    irr::scene::ITerrainSceneNode * getNode(const Terrain & terrain);
};


#endif //WORLD_TERRAINSCENENODE_H
