//
// Created by louis on 22/04/17.
//

#ifndef WORLD_APPLICATION_H
#define WORLD_APPLICATION_H

#include <atomic>
#include <memory>

#include "MainView.h"
#include "SynchronizedWorld.h"

class Application {
public:
    Application();

    void run(int argc, char** argv);
    void requestStop();

    SynchronizedWorld & getWorld();
private:
    std::atomic_bool _running;

    std::unique_ptr<MainView> _mainView;

    std::unique_ptr<SynchronizedWorld> _world;

    void loadWorld(int argc, char** argv);
};


#endif //WORLD_APPLICATION_H
