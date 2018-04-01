#include "Application.h"

#include <chrono>

#include <worldapi/world/FlatWorld.h>

#include "util.h"

using namespace world;

Application::Application()
        : _running(false),
	      _mainView(std::make_unique<MainView>(*this)),
		  _lastUpdatePos(0, 0, 0),
		  _explorer(std::make_unique<FirstPersonExplorer>(0.004)),
		  _collector(std::make_unique<SynchronizedCollector>()){

	_explorer->setOrigin({0, 0, 0});
}

void Application::run(int argc, char **argv) {
    loadWorld(argc, argv);

    _running = true;
    _mainView->show();

	bool firstExpand = true;

    while(_running) {
        if (!_mainView->running()) {
            _running = false;
        }
		else {
			// On prend les paramètres en local.
			_paramLock.lock();
			vec3d newUpdatePos = _newUpdatePos;
			_paramLock.unlock();

			if ((newUpdatePos - _lastUpdatePos).norm() > 50 || firstExpand) {
				_explorer->setOrigin(newUpdatePos);

				// Mise à jour du monde
				_collector->lock();
				FlatWorldCollector &collector = _collector->get();
				collector.reset();

				auto start = std::chrono::steady_clock::now();
				_explorer->explore<FlatWorld>(*_world, collector);
				
				if (_dbgOn) {
					std::cout << "Temps d'exploration : " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count() << " ms " << std::endl;
				}

				_collector->unlock();

				// Mise à jour de la vue
				_mainView->onWorldChange();
				_lastUpdatePos = newUpdatePos;

				firstExpand = false;
			}
		}

        sleep(20.0f);
    }

    _mainView->waitClose();
}

void Application::requestStop() {
    _running = false;
}

void Application::setUserPosition(vec3d pos) {
	_paramLock.lock();
	_newUpdatePos = pos;
	_paramLock.unlock();
}

vec3d Application::getUserPosition() const {
	_paramLock.lock();
	auto pos = _newUpdatePos;
	_paramLock.unlock();
	return pos;
}

SynchronizedCollector& Application::getCollector() {
	return *_collector;
}

void Application::loadWorld(int argc, char **argv) {
    _world = std::unique_ptr<FlatWorld>(FlatWorld::createDemoFlatWorld());
}