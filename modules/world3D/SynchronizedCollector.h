#ifndef WORLD_SYNCHRONIZEDWORLD_H
#define WORLD_SYNCHRONIZEDWORLD_H

#include <memory>
#include <mutex>
#include <atomic>

#include <worldcore.h>
#include <worldflat.h>

// TODO supprimer cette classe une fois que le collector sera synchronisé
class SynchronizedCollector {
public:
    SynchronizedCollector();

    world::FlatWorldCollector & get();
    void lock();
    void unlock();

private:
    std::mutex _mutex;
    std::atomic_bool _locked;
    std::unique_ptr<world::FlatWorldCollector> _collector;
};

#endif //WORLD_SYNCHRONIZEDWORLD_H
