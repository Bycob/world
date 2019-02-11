#include "MainView.h"

#include <iostream>

#include "Application.h"
#include "TerrainManager.h"
#include "ObjectsManager.h"
#include "WorldIrrlicht.h"
#include "SeaSceneNode.h"

using namespace irr;
using namespace irr::core;
using namespace irr::scene;
using namespace irr::video;

MainView::MainView(Application &app)
        : _app(app), _running(false), _resetScene(true), _worldChanged(false),
          _fpsModeActive(true) {}

MainView::~MainView() {}

void MainView::show() {
    _running = true;

    _graphicThread =
        std::make_unique<std::thread>(&MainView::runInternal, this);
}

bool MainView::running() { return _running; }

void MainView::waitClose() { _graphicThread->join(); }

void MainView::resetScene() { _resetScene = true; }

void MainView::onWorldChange() { _worldChanged = true; }

bool MainView::OnEvent(const SEvent &event) {
    if (event.EventType == EET_KEY_INPUT_EVENT) {
        if (event.KeyInput.PressedDown) {
            if (event.KeyInput.Key == KEY_ESCAPE) {
                _fpsModeActive = !_fpsModeActive;
                _camera->setInputReceiverEnabled(_fpsModeActive);
                _device->getCursorControl()->setVisible(!_fpsModeActive);
            } else if (event.KeyInput.Key == KEY_KEY_Q &&
                       event.KeyInput.Control) {
                _device->closeDevice();
            } else if (event.KeyInput.Key == KEY_KEY_S) {
                _cameraFast = !_cameraFast;
                float cameraSpeed = _cameraFast ? 1.f : 0.01f;
                auto anims = _camera->getAnimators();
                auto *fps = (ISceneNodeAnimatorCameraFPS *)(*anims.begin());
                fps->setMoveSpeed(cameraSpeed);
            }
        }
    }
    return false;
}


// Private implementation

void MainView::runInternal() {
    _device = irr::createDevice(irr::video::EDT_OPENGL,
                                irr::core::dimension2d<irr::u32>(1900, 1000),
                                32, false, true, true, this);

    _device->setResizable(true);
    _device->getLogger()->setLogLevel(ELOG_LEVEL::ELL_WARNING);
    _device->getCursorControl()->setVisible(false);

    _scenemanager = _device->getSceneManager();
    _driver = _device->getVideoDriver();

    // Initialisation des différents modules de rendu
    float cameraSpeed = _cameraFast ? 1.f : 0.005f;
    _camera = _scenemanager->addCameraSceneNodeFPS(0, 100.0f, cameraSpeed);
    _camera->setFOV(1.57);
    _camera->setNearValue(1.f);
    _camera->setFarValue(20000);
    _camera->setPosition(toIrrlicht(_app.getUserPosition()));
    //_camera = _scenemanager->addCameraSceneNode(0, vector3df(200 + 64, 200 +
    //119, 200 + 64), vector3df(64, 119, 64));

    /*/ ----- Tests (temporaire)
    auto cube = _scenemanager->addSphereSceneNode();
    cube->setMaterialFlag(EMF_LIGHTING, true);
    cube->getMaterial(0).AmbientColor.set(255, 30, 30, 30);
    cube->getMaterial(0).DiffuseColor.set(255, 255, 0, 0);
    cube->getMaterial(0).SpecularColor.set(255, 255, 255, 255);
    cube->getMaterial(0).ColorMaterial = ECM_NONE;//*/
    auto light = _scenemanager->addLightSceneNode(
        0, vector3df(0, 0, 1), video::SColorf(1.0f, 1.0f, 1.0f), 100.0f);
    light->setRotation(vector3df(60, 0, 0));
    light->getLightData().Type = ELT_DIRECTIONAL;

    irr::video::SColor bgColor(255, 190, 199, 220);
    auto fogColor = bgColor;
    fogColor.setAlpha(0);
    _driver->setFog(bgColor, irr::video::EFT_FOG_LINEAR, 2500, 10000, 0.01f,
                    false, true);

    // Affichage de debug
    _debug = new DebugScreenNode(_app, _device,
                                 _scenemanager->getRootSceneNode(), -1);
    auto *seaNode = new SeaSceneNode(_app, _device,
                                     _scenemanager->getRootSceneNode(), 100000);

    while (_device->run()) {
        updateScene();
        _debug->updateInfos();
        seaNode->updateInfos();

        _driver->beginScene(true, true, irr::video::SColor(255, 190, 199, 220));
        auto screenSize = _driver->getScreenSize();
        _camera->setAspectRatio((float)screenSize.Width / screenSize.Height);

        _scenemanager->drawAll();

        _driver->endScene();
    }

    _running = false;
    _resetScene = true;
}

void MainView::recreateModules() {
    _modules.clear();

    _modules.push_back(
        std::move(std::make_unique<TerrainManager>(_app, _device)));
    _modules.push_back(
        std::move(std::make_unique<ObjectsManager>(_app, _device)));
}

void MainView::updateScene() {
    auto camPos = _camera->getPosition();
    _app.setUserPosition(world::vec3d(camPos.X, camPos.Z, camPos.Y));

    auto collector = _app.popFull();

    if (!collector)
        return;

    if (_resetScene) {
        recreateModules();

        for (auto &module : _modules) {
            module->initialize(*collector);
        }

        _resetScene = false;
        _worldChanged = false;
    } else if (_worldChanged) {
        for (auto &module : _modules) {
            module->update(*collector);
        }

        _worldChanged = false;
    }

    _app.refill(std::move(collector));
}