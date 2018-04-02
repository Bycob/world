#ifndef WORLD_TERRAINSCENENODE_H
#define WORLD_TERRAINSCENENODE_H

#include <vector>
#include <map>
#include <utility>

#include <irrlicht.h>

#include <worldcore.h>
#include <worldflat.h>
#include <worldterrain.h>

#include "RenderingModule.h"

class GroundManager : public RenderingModule {
public:
    GroundManager(Application & app, irr::IrrlichtDevice * device);
	virtual ~GroundManager();

    void initialize(world::FlatWorldCollector &collector) override;
	void update(world::FlatWorldCollector &collector) override;
private:
	std::map<world::FlatWorldCollector::TerrainKey, irr::scene::ITerrainSceneNode*> _terrainNodes;

	void clearAllNodes();
    irr::scene::ITerrainSceneNode* createNode(const world::Terrain& terrain);
};


#endif //WORLD_TERRAINSCENENODE_H
