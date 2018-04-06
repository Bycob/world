#include "Application.h"

#include <chrono>

#include "util.h"

using namespace world;

Application::Application()
        : _running(false),
	      _mainView(std::make_unique<MainView>(*this)),
		  _newUpdatePos(0, 0, 5000),
		  _lastUpdatePos(_newUpdatePos),
		  _explorer(std::make_unique<FirstPersonExplorer>(1000)){

	_explorer->setPosition(_lastUpdatePos);
	_explorer->setFarDistance(10000);

	// Collectors
	for (int i = 0; i < 2; i++) {
		_emptyCollectors.emplace_back(std::make_unique<FlatWorldCollector>());
	}
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

			if (_emptyCollectors.empty()) {
				_mainView->onWorldChange();
			}
			_paramLock.unlock();

			if (((newUpdatePos - _lastUpdatePos).norm() > 50 || firstExpand) && !_emptyCollectors.empty()) {
			    // get collector
			    _paramLock.lock();
			    std::unique_ptr<FlatWorldCollector> collector = std::move(_emptyCollectors.front());
			    _emptyCollectors.pop_front();
			    _paramLock.unlock();

                // Mise à jour du monde
				collector->reset();
				_explorer->setPosition(newUpdatePos);

				auto start = std::chrono::steady_clock::now();
				_explorer->explore<FlatWorld>(*_world, *collector);
				
				if (_dbgOn) {
					std::cout << "Temps d'exploration : " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count() << " ms " << std::endl;
				}

				_paramLock.lock();
				_fullCollectors.emplace_back(std::move(collector));
				_paramLock.unlock();

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
    std::lock_guard<std::mutex> lock(_paramLock);
	_newUpdatePos = pos;
}

vec3d Application::getUserPosition() const {
    std::lock_guard<std::mutex> lock(_paramLock);
	auto pos = _newUpdatePos;
	return pos;
}

void Application::refill(std::unique_ptr<world::FlatWorldCollector> &&toRefill) {
    std::lock_guard<std::mutex> lock(_paramLock);
	_emptyCollectors.emplace_back(std::move(toRefill));
}

std::unique_ptr<world::FlatWorldCollector> Application::popFull() {
	std::lock_guard<std::mutex> lock(_paramLock);
	if (_fullCollectors.empty())
		return nullptr;

	auto ret = std::move(_fullCollectors.front());
	_fullCollectors.pop_front();

	return std::move(ret);
}

void Application::loadWorld(int argc, char **argv) {
    _world = std::unique_ptr<FlatWorld>(FlatWorld::createDemoFlatWorld());
}