//
// Created by louis on 30/04/17.
//

#ifndef WORLD_TERRAINSCENENODE_H
#define WORLD_TERRAINSCENENODE_H

#include <vector>
#include <map>
#include <utility>

#include <irrlicht.h>

#include <worldapi/terrain/Terrain.h>
#include <worldapi/terrain/Ground.h>
#include <worldapi/world/World.h>

#include "RenderingModule.h"

class GroundManager : public RenderingModule {
public:
    GroundManager(Application & app, irr::IrrlichtDevice * device);
	virtual ~GroundManager();

    void initialize(const World &world) override;
	void update(const World &world) override;
private:
	std::map<std::pair<int, int>, irr::scene::ITerrainSceneNode*> _terrainNodes;

	void clearAllNodes();
    void addNode(const TerrainTile & tile, const Ground & groundModule);
};


#endif //WORLD_TERRAINSCENENODE_H
