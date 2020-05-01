#include "Application.h"

#include <chrono>

#include "util.h"

using namespace world;

Application::Application()
        : _running(false), _resModel(std::make_unique<FirstPersonView>(700)),
          _newUpdatePos(0, 0, 5000), _lastUpdatePos(_newUpdatePos),
          _mainView(std::make_unique<MainView>(*this)) {

    _resModel->setPosition(_lastUpdatePos);
    _resModel->setFarDistance(10000);
    _lastUpdatePos = {0, 0, 0};

    // Collectors
    for (int i = 0; i < 2; i++) {
        auto collector = std::make_unique<Collector>(CollectorPresets::SCENE);
        _emptyCollectors.emplace_back(std::move(collector));
    }
}

void Application::run(int argc, char **argv) {
    loadWorld(argc, argv);

    _running = true;
    _mainView->show();

    while (_running) {
        if (!_mainView->running()) {
            _running = false;
        } else {
            // On prend les paramètres en local.
            _paramLock.lock();
            vec3d newUpdatePos = _newUpdatePos;

            if (_emptyCollectors.empty()) {
                _mainView->onWorldChange();
            }
            _paramLock.unlock();

            if ((newUpdatePos - _lastUpdatePos).norm() > 0.01 &&
                !_emptyCollectors.empty()) {
                // get collector
                _paramLock.lock();
                std::unique_ptr<Collector> collector =
                    std::move(_emptyCollectors.front());
                _emptyCollectors.pop_front();
                _paramLock.unlock();

                // Mise à jour du monde
                collector->reset();
                _resModel->setPosition(newUpdatePos);

                auto start = std::chrono::steady_clock::now();
                _world->collect(*collector, *_resModel);

                if (_dbgOn) {
                    std::cout << "Temps d'exploration : "
                              << std::chrono::duration_cast<
                                     std::chrono::milliseconds>(
                                     std::chrono::steady_clock::now() - start)
                                     .count()
                              << " ms " << std::endl;
                }

                _paramLock.lock();
                _fullCollectors.emplace_back(std::move(collector));
                _paramLock.unlock();

                // Mise à jour de la vue
                _mainView->onWorldChange();
                _lastUpdatePos = newUpdatePos;
            }
        }

        sleep(20.0f);
    }

    _mainView->waitClose();
}

void Application::requestStop() { _running = false; }

void Application::setUserPosition(vec3d pos) {
    std::lock_guard<std::mutex> lock(_paramLock);
    _newUpdatePos = pos;
}

vec3d Application::getUserPosition() const {
    std::lock_guard<std::mutex> lock(_paramLock);
    auto pos = _newUpdatePos;
    return pos;
}

void Application::refill(std::unique_ptr<world::Collector> &&toRefill) {
    std::lock_guard<std::mutex> lock(_paramLock);
    _emptyCollectors.emplace_back(std::move(toRefill));
}

std::unique_ptr<world::Collector> Application::popFull() {
    std::lock_guard<std::mutex> lock(_paramLock);
    if (_fullCollectors.empty())
        return nullptr;

    auto ret = std::move(_fullCollectors.front());
    _fullCollectors.pop_front();

    return std::move(ret);
}

#ifdef USE_VKWORLD
#include "vkworld/VkWorld.h"
#endif

void Application::loadWorld(int argc, char **argv) {
    bool vulkan = false;

    if (argc > 1) {
        std::string arg = argv[1];

        if (arg == "vulkan") {
            vulkan = true;
        }
        // Try read the file
        else {
            std::cout << "Loading world " << arg << std::endl;
            _world = std::make_unique<FlatWorld>();
            _world->load(arg);
            // _world->save("dump.json");
            return;
        }
    }

#ifdef USE_VKWORLD
    if (vulkan) {
        _world = std::unique_ptr<FlatWorld>(VkWorld::createDemoFlatWorld());
    } else {
        _world = std::unique_ptr<FlatWorld>(FlatWorld::createDemoFlatWorld());
    }
#else
    if (vulkan) {
        std::cout << "Vulkan not supported, fallback to default world"
                  << std::endl;
    }
    _world = std::unique_ptr<FlatWorld>(FlatWorld::createDemoFlatWorld());
#endif
}