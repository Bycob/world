//
// Created by louis on 26/04/17.
//

#include "SynchronizedWorld.h"

SynchronizedWorld * SynchronizedWorld::createFromWorld(World * world) {
	SynchronizedWorld * syncWorld = new SynchronizedWorld();
	syncWorld->_world = std::unique_ptr<World>(world);
	return syncWorld;
}

//private 
SynchronizedWorld::SynchronizedWorld() {

}

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