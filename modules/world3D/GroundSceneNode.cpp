//
// Created by louis on 30/04/17.
//

#include "GroundSceneNode.h"

#include <worldapi/terrain/TerrainGenerator.h>

using namespace irr;
using namespace core;
using namespace scene;
using namespace io;
using namespace video;

GroundSceneNode::GroundSceneNode(IrrlichtDevice *device)
        : _sceneManager(device->getSceneManager()),
          _fileSystem(device->getFileSystem()){

}

void GroundSceneNode::initialize(const World &world) {
    const Ground & ground = world.getUniqueNode<Ground>();

    // Test
    PerlinTerrainGenerator generator(129, 0, 5, 1, 0.4);
    std::unique_ptr<Terrain> terrain(generator.generate());
	addNode({ *terrain, 0, 0 }, ground);
    //*/
}

void GroundSceneNode::addNode(const TerrainTile &tile, const Ground & groundModule) {
	const Terrain & terrain = tile._terrain;

	// Données concernant la position et les dimensions du terrain
	int terrainRes = terrain.getSize();
	double tileSize = groundModule.getUnitSize();
	double scaling = tileSize / terrainRes;
	double terrainX = tile._x * tileSize;
	double terrainY = tile._y * tileSize;

	double heightScaling = groundModule.getAltitudeRange();
	double heightPos = groundModule.getMinAltitude();

	// Construction du noeud irrlicht
    int dataSize;
    const char* data = terrain.getRawData(dataSize, heightScaling / scaling);
    IReadFile * memoryFile = _fileSystem->createMemoryReadFile((void*)data, dataSize, "", false);

    ITerrainSceneNode * result =
            _sceneManager->addTerrainSceneNode(nullptr, 0, -1,
                                               vector3df(terrainX, heightPos, terrainY),
                                               vector3df(0.0f, 0.0f, 0.0f),
                                               vector3df(scaling, scaling, scaling),
                                               SColor(255, 255, 255, 255),
                                               5, ETPS_17, 4, true);
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
    material.DiffuseColor.set(255, 123, 85, 12);


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

	_terrainNodes.push_back(result);
}