#include "Application.h"

#include <worldapi/world/WorldGenerator.h>

#include "util.h"

using namespace maths;

Application::Application()
        : _running(false),
	      _mainView(std::make_unique<MainView>(*this)), 
	      _userPos(vec3d(0, 0, 0), 3000),
		  _lastUpdatePos(0, 0, 0) {

}

void Application::run(int argc, char **argv) {
    loadWorld(argc, argv);

    _running = true;
    _mainView->show();

    while(_running) {
        if (!_mainView->running()) {
            _running = false;
        }
		else {
			// On prend les paramètres en local.
			_paramLock.lock();
			PointOfView userPos = _userPos;
			_paramLock.unlock();

			if ((userPos._pos - _lastUpdatePos).norm() > 1000) {
				_world->lock();
				World & world = _world->get();
				world.getGenerator().expand(world, userPos);

				_world->unlock();

				_mainView->resetScene();
				_lastUpdatePos = userPos._pos;
			}
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

void Application::setUserPosition(maths::vec3d pos) {
	_paramLock.lock();
	_userPos._pos = pos;
	_paramLock.unlock();
}

void Application::loadWorld(int argc, char **argv) {
    _world = std::make_unique<SynchronizedWorld>(*WorldGenerator::defaultGenerator());

    _world->lock();
    World & world = _world->get();
    // Do some initialization stuff

    _world->unlock();
}