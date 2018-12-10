#ifndef WORLD_TERRAINSCENENODE_H
#define WORLD_TERRAINSCENENODE_H

#include <vector>
#include <map>
#include <utility>

#include <irrlicht.h>

#include <world/core.h>
#include <world/flat.h>
#include <world/terrain.h>

#include "RenderingModule.h"

class TerrainManager : public RenderingModule {
public:
    TerrainManager(Application & app, irr::IrrlichtDevice * device);
	virtual ~TerrainManager();

    void initialize(world::Collector &collector) override;
	void update(world::Collector &collector) override;
private:
	std::map<world::ItemKey, irr::scene::ITerrainSceneNode*> _terrainNodes;

	void clearAllNodes();
    irr::scene::ITerrainSceneNode* createNode(const world::Terrain& terrain);
};


#endif //WORLD_TERRAINSCENENODE_H
