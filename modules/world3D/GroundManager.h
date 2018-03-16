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

    void initialize(FlatWorldCollector &collector) override;
	void update(FlatWorldCollector &collector) override;
private:
	std::map<FlatWorldCollector::TerrainKey, irr::scene::ITerrainSceneNode*> _terrainNodes;

	void clearAllNodes();
    irr::scene::ITerrainSceneNode* createNode(const Terrain& terrain);
};


#endif //WORLD_TERRAINSCENENODE_H
