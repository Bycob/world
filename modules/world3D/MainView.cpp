//
// Created by louis on 22/04/17.
//

#include "MainView.h"

#include <iostream>

#include "Application.h"

MainView::MainView(Application & app)
        : _app(app), _running(false) {

    _device = irr::createDevice (
            irr::video::EDT_OPENGL,
            irr::core::dimension2d<irr::u32>(800,600),
            32,
            false,
            true,
            true,
            0);
    _scenemanager = _device->getSceneManager () ;
    _driver = _device->getVideoDriver();
}

MainView::~MainView() {

}

void MainView::show() {
    _graphicThread = std::make_unique<std::thread>(&MainView::runInternal, this);
}

void MainView::waitClose() {
    _graphicThread->join();
}

void MainView::runInternal() {

    while(_device->run()) {
        _lock.lock();

        _driver->beginScene(true, true,
                            irr::video::SColor(255, 255, 255, 255));
        _scenemanager->drawAll();
        _driver->endScene();

        _lock.unlock();
    }

    _device->closeDevice();
    _app.requestStop();
}