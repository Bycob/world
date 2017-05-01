//
// Created by louis on 30/04/17.
//

#include "GroundSceneNode.h"

#include <worldapi/world/Ground.h>

using namespace irr;
using namespace scene;
using namespace io;

GroundSceneNode::GroundSceneNode(IrrlichtDevice *device)
        : _sceneManager(device->getSceneManager()),
          _fileSystem(device->getFileSystem()){

}

void GroundSceneNode::initialize(const World &world) {
    const Ground & ground = world.getUniqueNode<Ground>();

    // get a stream on the ground;

    //IReadFile * rawFile = _fileSystem->createMemoryReadFile();
}

ITerrainSceneNode* GroundSceneNode::getNode(const Terrain &terrain) {
    std::stringstream stream;
    terrain.writeRawData(stream);
    // TODO faire un vrai buffer pour éviter une copie inutile
    const std::string & str = stream.str();
    const char* data = str.c_str();
    IReadFile * memoryFile = _fileSystem->createMemoryReadFile((void*)data, str.size(), "", false);

    ITerrainSceneNode * result = _sceneManager->addTerrainSceneNode(memoryFile);
    return result;
}