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
    PerlinTerrainGenerator generator;
    std::unique_ptr<Terrain> terrain(generator.generate());
    getNode(*terrain);
    //*/
}

ITerrainSceneNode* GroundSceneNode::getNode(const Terrain &terrain) {
    std::stringstream stream;
    terrain.writeRawData(stream);
    // TODO faire un vrai buffer pour éviter une copie inutile
    const std::string & str = stream.str();
    const char* data = str.c_str();
    IReadFile * memoryFile = _fileSystem->createMemoryReadFile((void*)data, terrain.getRawDataSize(), "", false);

    ITerrainSceneNode * result =
            _sceneManager->addTerrainSceneNode("tests/perlin1.png", 0, -1,
                                               vector3df(0.0f, 0.0f, 0.0f),
                                               vector3df(0.0f, 0.0f, 0.0f),
                                               vector3df(1.0f, 0.5f, 1.0f),
                                               SColor(255, 255, 255, 255),
                                               5, ETPS_17, 4, true);
    //result->loadHeightMapRAW(memoryFile, 32, true, true, 0, SColor(255,255,255,255), 4);
    result->setMaterialFlag(video::EMF_LIGHTING, true);

    result->setMaterialType(EMT_SOLID);
    auto &material = result->getMaterial(0);
    material.BackfaceCulling = false;
    material.AmbientColor = SColor(255, 50, 50, 50);
    material.SpecularColor = SColor(255, 255, 255, 0);
    material.DiffuseColor = SColor(255, 255, 240, 123);


    // Collision pour la camera
    scene::ITriangleSelector* selector
            = _sceneManager->createTerrainTriangleSelector(result, 0);
    result->setTriangleSelector(selector);

    // create collision response animator and attach it to the camera
    scene::ISceneNodeAnimator* anim = _sceneManager->createCollisionResponseAnimator(
            selector, _sceneManager->getActiveCamera(), core::vector3df(0.5,1,0.6),
            core::vector3df(0,-1,0),
            core::vector3df(0,0.5,0));
    selector->drop();
    _sceneManager->getActiveCamera()->addAnimator(anim);
    anim->drop();
    return result;
}