#ifndef WORLD_SEASCENENODE_H
#define WORLD_SEASCENENODE_H
#pragma once

#include <irrlicht.h>

#include <world/core.h>

class Application;

class SeaSceneNode : public irr::scene::ISceneNode {
public:
    SeaSceneNode(Application &app, irr::IrrlichtDevice *device,
                 irr::scene::ISceneNode *parent, irr::s32 id);

    void updateInfos();
    void setVisible(bool visible);

    virtual void OnRegisterSceneNode();
    virtual void render();

    const irr::core::aabbox3d<irr::f32> &getBoundingBox() const override {
        return _box;
    }
    irr::u32 getMaterialCount() const override { return 1; }
    irr::video::SMaterial &getMaterial(irr::u32 i) override {
        return _material;
    }

private:
    Application &_app;
    irr::IrrlichtDevice *_device;

    irr::core::aabbox3d<irr::f32> _box;
    irr::video::SMaterial _material;

    world::vec3d _userPos;
};


#endif // WORLD_SEASCENENODE_H
