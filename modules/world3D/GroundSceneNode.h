//
// Created by louis on 30/04/17.
//

#ifndef WORLD_TERRAINSCENENODE_H
#define WORLD_TERRAINSCENENODE_H

#include <vector>
#include <map>
#include <utility>

#include <irrlicht.h>

#include <worldapi/terrain/terrain.h>
#include <worldapi/terrain/Ground.h>
#include <worldapi/world/World.h>

class Application;

class GroundSceneNode {
public:
    GroundSceneNode(Application & app, irr::IrrlichtDevice * device);
	virtual ~GroundSceneNode();

    void initialize(const World &world);
	void update(const World &world);
private:
	Application & _app;

    irr::scene::ISceneManager * _sceneManager;
	irr::video::IVideoDriver *_driver;
    irr::io::IFileSystem * _fileSystem;

	std::map<std::pair<int, int>, irr::scene::ITerrainSceneNode*> _terrainNodes;

	void clearAllNodes();
    void addNode(const TerrainTile & tile, const Ground & groundModule);
};


#endif //WORLD_TERRAINSCENENODE_H
