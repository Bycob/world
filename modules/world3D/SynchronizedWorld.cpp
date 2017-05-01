//
// Created by louis on 26/04/17.
//

#include "SynchronizedWorld.h"

World& SynchronizedWorld::get() {
    if (!_locked) {
        throw std::runtime_error("You may lock the world before getting it.");
    }
    return *_world;
}

void SynchronizedWorld::lock() {
    _mutex.lock();
    _locked = true;
}

void SynchronizedWorld::unlock() {
    _locked = false;
    _mutex.unlock();
}