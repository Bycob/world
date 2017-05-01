//
// Created by louis on 22/04/17.
//

#include "MainView.h"

#include <iostream>

#include "Application.h"

using namespace irr;
using namespace irr::core;
using namespace irr::scene;
using namespace irr::video;

MainView::MainView(Application & app)
        : _app(app), _running(false), _resetScene(true) {

}

MainView::~MainView() {

}

void MainView::show() {
    _running = true;

    _graphicThread = std::make_unique<std::thread>(&MainView::runInternal, this);
}

bool MainView::running() {
    return _running;
}

void MainView::waitClose() {
    _graphicThread->join();
}

void MainView::runInternal() {
    _device = irr::createDevice (
            irr::video::EDT_OPENGL,
            irr::core::dimension2d<irr::u32>(800,600),
            32,
            false,
            true,
            true,
            0);
    _scenemanager = _device->getSceneManager();
    _driver = _device->getVideoDriver();

    // Initialisation des différents modules de rendu
    _camera = _scenemanager->addCameraSceneNodeMaya();
    _camera->setPosition(vector3df(5.0, 0.0, 0.0));
    _camera->setTarget(vector3df(0, 0, 0));

    /*// ----- Tests (temporaire)
    auto cube = _scenemanager->addSphereSceneNode();
    cube->setMaterialFlag(EMF_LIGHTING, true);
    cube->getMaterial(0).AmbientColor.set(255, 30, 30, 30);
    cube->getMaterial(0).DiffuseColor.set(255, 255, 0, 0);
    cube->getMaterial(0).SpecularColor.set(255, 255, 255, 255);
    auto light = _scenemanager->addLightSceneNode(cube, vector3df(0, 8, 0), video::SColorf(1.0f,1.0f,1.0f), 745.0f, 1);
    light->getLightData().Type = ELT_POINT;
    light->getLightData().Attenuation.set(1, 0, 0);
    //_scenemanager->setAmbientLight(SColorf(1.0f, 1.0f, 1.0f));*/

    while(_device->run()) {
        updateScene();

        _driver->beginScene(true, true,
                            irr::video::SColor(255, 5, 35, 0));
        auto screenSize = _driver->getScreenSize();
        _camera->setAspectRatio((float) screenSize.Width / screenSize.Height);

        _scenemanager->drawAll();

        _driver->endScene();
    }

    _device->closeDevice();

    _running = false;
    _resetScene = true;
}

void MainView::updateScene() {
    if (_resetScene) {
        SynchronizedWorld & syncWorld = _app.getWorld();
        syncWorld.lock();
        World & world = syncWorld.get();

        _ground = std::make_unique<GroundSceneNode>(_device);
        _ground->initialize(world);

        syncWorld.unlock();

        _resetScene = false;
    }
    else {

    }
}