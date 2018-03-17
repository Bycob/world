//
// Created by louis on 26/04/17.
//

#include "SynchronizedCollector.h"

//private 
SynchronizedCollector::SynchronizedCollector()
        : _collector(std::make_unique<world::FlatWorldCollector>()) {

}

world::FlatWorldCollector& SynchronizedCollector::get() {
    if (!_locked) {
        throw std::runtime_error("You may lock the world before getting it.");
    }
    return *_collector;
}

void SynchronizedCollector::lock() {
    _mutex.lock();
    _locked = true;
}

void SynchronizedCollector::unlock() {
    _locked = false;
    _mutex.unlock();
}