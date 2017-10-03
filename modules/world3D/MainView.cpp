//
// Created by louis on 22/04/17.
//

#include "MainView.h"

#include <iostream>

#include "Application.h"
#include "GroundManager.h"
#include "ObjectsManager.h"

using namespace irr;
using namespace irr::core;
using namespace irr::scene;
using namespace irr::video;

MainView::MainView(Application & app)
	: _app(app), _running(false), _resetScene(true), _worldChanged(false), _fpsModeActive(true) {

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

void MainView::resetScene() {
	_resetScene = true;
}

void MainView::onWorldChange() {
	_worldChanged = true;
}

bool MainView::OnEvent(const SEvent& event) {
	if (event.EventType == EET_KEY_INPUT_EVENT) {
		if (event.KeyInput.PressedDown) {
			if (event.KeyInput.Key == KEY_ESCAPE) {
				_fpsModeActive = !_fpsModeActive;
				_camera->setInputReceiverEnabled(_fpsModeActive);
				_device->getCursorControl()->setVisible(!_fpsModeActive);
			}
			else if (event.KeyInput.Key == KEY_KEY_Q) {
				_device->closeDevice();
			}
		}
	}
	return false;
}


// Private implementation

void MainView::runInternal() {
    _device = irr::createDevice (
            irr::video::EDT_OPENGL,
            irr::core::dimension2d<irr::u32>(1900,1000),
            32,
            false,
            true,
            true,
            this);

	_device->setResizable(true);
    _device->getLogger()->setLogLevel(ELOG_LEVEL::ELL_WARNING);
    _device->getCursorControl()->setVisible(false);

    _scenemanager = _device->getSceneManager();
    _driver = _device->getVideoDriver();

    // Initialisation des différents modules de rendu
    _camera = _scenemanager->addCameraSceneNodeFPS(0, 100.0f, 5.0f);
	_camera->setFOV(1.57);
    _camera->setFarValue(40000);
    _camera->setPosition(vector3df(0, 1200, 500));
    //_camera = _scenemanager->addCameraSceneNode(0, vector3df(200 + 64, 200 + 119, 200 + 64), vector3df(64, 119, 64));
	
    /*/ ----- Tests (temporaire)
    auto cube = _scenemanager->addSphereSceneNode();
    cube->setMaterialFlag(EMF_LIGHTING, true);
    cube->getMaterial(0).AmbientColor.set(255, 30, 30, 30);
    cube->getMaterial(0).DiffuseColor.set(255, 255, 0, 0);
    cube->getMaterial(0).SpecularColor.set(255, 255, 255, 255);
    cube->getMaterial(0).ColorMaterial = ECM_NONE;//*/
    auto light = _scenemanager->addLightSceneNode(0, vector3df(0, 200, 0), video::SColorf(1.0f,1.0f,1.0f), 745.0f);
    light->setRotation(vector3df(45, 0, 0));
    light->getLightData().Type = ELT_DIRECTIONAL;

	// Affichage de debug
	_debug = new DebugScreenNode(_app, _device, _scenemanager->getRootSceneNode(), -1);

    while(_device->run()) {
        updateScene();
		_debug->updateInfos();

        _driver->beginScene(true, true,
                            irr::video::SColor(255, 190, 199, 220));
        auto screenSize = _driver->getScreenSize();
        _camera->setAspectRatio((float) screenSize.Width / screenSize.Height);

        _scenemanager->drawAll();

        _driver->endScene();
    }

    _device->closeDevice();

    _running = false;
    _resetScene = true;
}

void MainView::recreateModules() {
    _modules.clear();

    _modules.push_back(std::move(std::make_unique<GroundManager>(_app, _device)));
    _modules.push_back(std::move(std::make_unique<ObjectsManager>(_app, _device)));
}

void MainView::updateScene() {
	SynchronizedWorld & syncWorld = _app.getWorld();

	auto camPos = _camera->getPosition();
	_app.setUserPosition(maths::vec3d(camPos.X, camPos.Z, camPos.Y));

    if (_resetScene) {

		syncWorld.lock();
		World & world = syncWorld.get();

        recreateModules();

        for (auto & module : _modules) {
            module->initialize(world);
        }

        _resetScene = false;
        _worldChanged = false;

		syncWorld.unlock();
    }
    else if (_worldChanged) {
        syncWorld.lock();
        World & world = syncWorld.get();

        for (auto & module : _modules) {
            module->update(world);
        }

        _worldChanged = false;

        syncWorld.unlock();
    }
}