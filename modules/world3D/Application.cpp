#include "Application.h"

#include "util.h"

Application::Application()
        : _running(false), _mainView(std::make_unique<MainView>(*this)) {

}

void Application::run(int argc, char **argv) {
    loadWorld(argc, argv);

    _running = true;
    _mainView->show();

    while(_running) {
        sleep(20);
    }

    _mainView->waitClose();
}

void Application::requestStop() {
    _running = false;
}

const SynchronizedWorld & Application::getWorld() const {
    if (_world == nullptr) throw std::runtime_error("getWorld() called while not running.");
    return *_world;
}

void Application::loadWorld(int argc, char **argv) {
    _world = std::make_unique<SynchronizedWorld>();

    _world->lock();
    World & world = _world->get();
    // Do some initialization stuff

    _world->unlock();
}