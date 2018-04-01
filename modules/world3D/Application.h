//
// Created by louis on 22/04/17.
//

#ifndef WORLD_APPLICATION_H
#define WORLD_APPLICATION_H

#include <atomic>
#include <mutex>
#include <memory>

#include <worldapi/world/FlatWorld.h>
#include <worldapi/world/FirstPersonExplorer.h>
#include <worldapi/world/FlatWorldCollector.h>

#include "MainView.h"
#include "SynchronizedCollector.h"

class Application {
public:
    Application();

    void run(int argc, char** argv);
    void requestStop();

	void setUserPosition(world::vec3d pos);
	world::vec3d getUserPosition() const;

	SynchronizedCollector & getCollector();
private:
    std::atomic_bool _running;
	mutable std::mutex _paramLock;

	std::unique_ptr<world::FlatWorld> _world;
	std::unique_ptr<world::FirstPersonExplorer> _explorer;
	std::unique_ptr<SynchronizedCollector> _collector;

	world::vec3d _newUpdatePos;
	world::vec3d _lastUpdatePos;

    std::unique_ptr<MainView> _mainView;

	bool _dbgOn = true;

    void loadWorld(int argc, char** argv);
};


#endif //WORLD_APPLICATION_H
