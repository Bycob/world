//
// Created by louis on 26/04/17.
//

#ifndef WORLD_SYNCHRONIZEDWORLD_H
#define WORLD_SYNCHRONIZEDWORLD_H

#include <memory>
#include <mutex>
#include <atomic>

#include <worldapi/world/World.h>

class SynchronizedWorld {
public:
    SynchronizedWorld();

    World & get();
    void lock();
    void unlock();

private:
    std::mutex _mutex;
    std::atomic_bool _locked;
    std::unique_ptr<World> _world;
};


#endif //WORLD_SYNCHRONIZEDWORLD_H
