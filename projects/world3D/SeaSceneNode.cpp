#include "SeaSceneNode.h"

#include "Application.h"

using namespace irr;
using namespace irr::scene;
using namespace irr::video;
using namespace irr::core;

SeaSceneNode::SeaSceneNode(
        Application &app, IrrlichtDevice *device,
        ISceneNode *parent, s32 id)
        : ISceneNode(parent, device->getSceneManager(), id), _app(app), _device(device) {

    setMaterialType(EMT_TRANSPARENT_ADD_COLOR);

    //_material.BlendOperation = EBO_ADD;
    _material.BackfaceCulling = false;

    setAutomaticCulling(EAC_OFF);
}

void SeaSceneNode::updateInfos() {
    _userPos = _app.getUserPosition();
}

void SeaSceneNode::setVisible(bool visible) {
    ISceneNode::setVisible(visible);
}

void SeaSceneNode::OnRegisterSceneNode() {
    if (IsVisible)
        SceneManager->registerNodeForRendering(this);

    ISceneNode::OnRegisterSceneNode();
}

void SeaSceneNode::render() {

    float bounds[] {
        static_cast<float>(_userPos.x - 10000),
        static_cast<float>(_userPos.y - 10000),
        static_cast<float>(_userPos.x + 10000),
        static_cast<float>(_userPos.y + 10000)
    };

    _box.reset(vector3df{bounds[0], 0, bounds[1]});
    _box.reset(vector3df{bounds[2], 0, bounds[3]});

    SColor seaColor(50,0,0,255);

    S3DVertex vertices[4] {
        S3DVertex(bounds[0],0,bounds[1], 0,1,0, seaColor, 0, 1),
        S3DVertex(bounds[2],0,bounds[1], 0,1,0, seaColor, 0, 1),
        S3DVertex(bounds[2],0,bounds[3], 0,1,0, seaColor, 0, 1),
        S3DVertex(bounds[0],0,bounds[3], 0,1,0, seaColor, 0, 1),
    };
    u16 indices[] = { 0,1,2, 0,3,2};
    video::IVideoDriver* driver = SceneManager->getVideoDriver();

    driver->setMaterial(_material);
    driver->setTransform(video::ETS_WORLD, AbsoluteTransformation);
    driver->drawVertexPrimitiveList(&vertices[0], 4, &indices[0], 2, video::EVT_STANDARD, scene::EPT_TRIANGLES, video::EIT_16BIT);
}
