//
// Created by louis on 22/04/17.
//

#ifndef WORLD_APPLICATION_H
#define WORLD_APPLICATION_H

#include <atomic>
#include <mutex>
#include <memory>

#include <worldapi/world/IPointOfView.h>

#include "MainView.h"
#include "SynchronizedWorld.h"

class Application {
public:
    Application();

    void run(int argc, char** argv);
    void requestStop();

    SynchronizedWorld & getWorld();

	void setUserPosition(maths::vec3d pos);
	PointOfView getUserPointOfView() const;
private:
    std::atomic_bool _running;
	mutable std::mutex _paramLock;

    std::unique_ptr<MainView> _mainView;

    std::unique_ptr<SynchronizedWorld> _world;
	PointOfView _userPos;

	maths::vec3d _lastUpdatePos;

    void loadWorld(int argc, char** argv);
};


#endif //WORLD_APPLICATION_H
