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