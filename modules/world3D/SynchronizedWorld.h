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
	static SynchronizedWorld * createFromWorld(World * world);

    template <typename... Args>
    SynchronizedWorld(Args... args);

    World & get();
    void lock();
    void unlock();

private:
	SynchronizedWorld();

    std::mutex _mutex;
    std::atomic_bool _locked;
    std::unique_ptr<World> _world;
};

template <typename... Args>
SynchronizedWorld::SynchronizedWorld(Args... args)
        : _world(std::make_unique<World>(args...)), _locked(false) {

}

#endif //WORLD_SYNCHRONIZEDWORLD_H
