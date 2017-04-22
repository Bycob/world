//
// Created by louis on 22/04/17.
//

#ifndef WORLD_APPLICATION_H
#define WORLD_APPLICATION_H

#include <atomic>
#include <memory>

#include "MainView.h"

class Application {
public:
    Application(int argc, char** argv);

    void run();
    void requestStop();
private:
    std::atomic_bool _running;

    std::unique_ptr<MainView> _mainView;
};


#endif //WORLD_APPLICATION_H
