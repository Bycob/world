#include "Application.h"

#include <worldapi/world/WorldGenerator.h>

#include "util.h"

Application::Application()
        : _running(false), _mainView(std::make_unique<MainView>(*this)) {

}

void Application::run(int argc, char **argv) {
    loadWorld(argc, argv);

    _running = true;
    _mainView->show();

    while(_running) {
        if (!_mainView->running()) {
            _running = false;
        }

        sleep(20.0f);
    }

    _mainView->waitClose();
}

void Application::requestStop() {
    _running = false;
}

SynchronizedWorld & Application::getWorld() {
    if (_world == nullptr) throw std::runtime_error("getWorld() called while not running.");
    return *_world;
}

void Application::loadWorld(int argc, char **argv) {
    _world = std::make_unique<SynchronizedWorld>(*WorldGenerator::defaultGenerator());

    _world->lock();
    World & world = _world->get();
    // Do some initialization stuff

    _world->unlock();
}