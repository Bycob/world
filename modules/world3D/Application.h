//
// Created by louis on 22/04/17.
//

#ifndef WORLD_APPLICATION_H
#define WORLD_APPLICATION_H

#include <atomic>
#include <mutex>
#include <memory>
#include <list>

#include <worldcore.h>
#include <worldflat.h>

#include "MainView.h"

class Application {
public:
    Application();

    void run(int argc, char** argv);
    void requestStop();

	void setUserPosition(world::vec3d pos);
	world::vec3d getUserPosition() const;

	void refill(std::unique_ptr<world::FlatWorldCollector> && toRefill);
	std::unique_ptr<world::FlatWorldCollector> popFull();
private:
    std::atomic_bool _running;
	mutable std::mutex _paramLock;

	std::unique_ptr<world::FlatWorld> _world;
	std::unique_ptr<world::FirstPersonExplorer> _explorer;

	std::list<std::unique_ptr<world::FlatWorldCollector>> _emptyCollectors;
	std::list<std::unique_ptr<world::FlatWorldCollector>> _fullCollectors;

	world::vec3d _newUpdatePos;
	world::vec3d _lastUpdatePos;

    std::unique_ptr<MainView> _mainView;

	bool _dbgOn = true;

    void loadWorld(int argc, char** argv);
};


#endif //WORLD_APPLICATION_H
