//
// Created by louis on 30/04/17.
//

#include "GroundManager.h"

#include <worldapi/world/FlatWorld.h>
#include <worldapi/terrain/TerrainGenerator.h>

#include "Application.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace io;
using namespace video;

using namespace world;

using TerrainKey = FlatWorldCollector::TerrainKey;

GroundManager::GroundManager(Application & application, IrrlichtDevice *device)
        : RenderingModule(application, device) {

}

GroundManager::~GroundManager() {
    clearAllNodes();
}

void GroundManager::initialize(FlatWorldCollector &collector) {
    clearAllNodes();

    update(collector);
}

void GroundManager::update(FlatWorldCollector &collector) {

    /*/ Test
    PerlinTerrainGenerator generator(0, 5, 1, 0.4);
    std::unique_ptr<Terrain> terrain(generator.generate());
	addNode({ *terrain, 0, 0 }, ground);
    //*/
    std::map<TerrainKey, bool> toKeep;

    // Add terrains if they're not already here
    auto terrainIt = collector.iterateTerrains();

    for (; terrainIt.hasNext(); ++terrainIt) {
        auto pair = *terrainIt;

        if (_terrainNodes.find(pair.first) == _terrainNodes.end()) {
            _terrainNodes[pair.first] = createNode(*pair.second);
        }
        toKeep[pair.first] = true;
    }

    // Remove terrains that should be removed
    auto iter = _terrainNodes.begin();

    for (; iter != _terrainNodes.end();) {
        if (toKeep.find(iter->first) == toKeep.end()) {
            iter->second->remove();
            iter = _terrainNodes.erase(iter);
        }
        else {
            ++iter;
        }
    }

    _driver->removeAllHardwareBuffers();
}

void GroundManager::clearAllNodes() {
    for (auto & pair : _terrainNodes) {
        pair.second->remove();
    }

    _terrainNodes.clear();
}

ITerrainSceneNode* GroundManager::createNode(const Terrain &terrain) {

	// Données concernant la position et les dimensions du terrain
	int terrainRes = (terrain.getSize() - 1);
	vec3d offset = terrain.getBoundingBox().getLowerBound();
	vec3d size = terrain.getBoundingBox().getUpperBound() - offset;
    size = size / terrainRes;

	// Construction du noeud irrlicht
    int dataSize;
    const char* data = terrain.getRawData(dataSize, (float) (terrainRes * size.z / size.x));
    IReadFile * memoryFile = _fileSystem->createMemoryReadFile((void*)data, dataSize, "", false);

    ITerrainSceneNode * result =
            _sceneManager->addTerrainSceneNode(nullptr, nullptr, -1,
                                               vector3df((float) offset.x, (float) offset.z, (float) offset.y),
                                               vector3df(0.0f, 0.0f, 0.0f),
                                               vector3df((float) size.x, (float) size.x, (float) size.x),
                                               SColor(255, 255, 255, 255),
                                               2, ETPS_17, 4, true);
    result->loadHeightMapRAW(memoryFile, 32, true, true, 0, SColor(255,255,255,255), 4);

	memoryFile->drop();
	delete[] data;

    // Paramètres d'affichage
    result->setMaterialFlag(video::EMF_LIGHTING, true);
    result->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true);

    result->setMaterialType(EMT_SOLID);
    auto &material = result->getMaterial(0);
    material.BackfaceCulling = false;
    material.ColorMaterial = ECM_NONE;

	// Couleurs du terrain
    material.AmbientColor.set(255, 0, 0, 0);
    material.SpecularColor.set(255, 255, 255, 255);

    irr::u32 lin_ts = (irr::u32)(log(size.x / 250) / log(2) * 50);
    material.DiffuseColor.set(255, 255 - lin_ts, 178, lin_ts);
    //material.DiffuseColor.set(255, 200, 178, 126);

    /*/ Collision pour la camera
    scene::ITriangleSelector* selector
            = _sceneManager->createTerrainTriangleSelector(result, 0);
    result->setTriangleSelector(selector);

    // create collision response animator and attach it to the camera
    scene::ISceneNodeAnimatorCollisionResponse* anim = _sceneManager->createCollisionResponseAnimator(
            selector, _sceneManager->getActiveCamera(), core::vector3df(1,2,1),
            core::vector3df(0,-0,0),
            core::vector3df(0,0.5,0));
    selector->drop();
    _sceneManager->getActiveCamera()->addAnimator(anim);
    anim->drop();
	//*/

	return result;
}