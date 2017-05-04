//
// Created by louis on 30/04/17.
//

#include "GroundSceneNode.h"

#include <worldapi/world/Ground.h>
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
    PerlinTerrainGenerator generator(513, 0, 5, 2, 0.4);
    std::unique_ptr<Terrain> terrain(generator.generate());
    getNode(*terrain);
    //*/
}

ITerrainSceneNode* GroundSceneNode::getNode(const Terrain &terrain) {
    // Creation du terrain
    int dataSize;
    const char* data = terrain.getRawData(dataSize, 1000);
    IReadFile * memoryFile = _fileSystem->createMemoryReadFile((void*)data, dataSize, "", false);

    ITerrainSceneNode * result =
            _sceneManager->addTerrainSceneNode(nullptr, 0, -1,
                                               vector3df(0.0f, 0.0f, 0.0f),
                                               vector3df(0.0f, 0.0f, 0.0f),
                                               vector3df(1.0f, 1.0f, 1.0f),
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
    material.AmbientColor.set(255, 0, 0, 0);
    material.SpecularColor.set(255, 255, 255, 255);
    material.DiffuseColor.set(255, 123, 85, 12);


    // Collision pour la camera
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
    return result;
}