#ifndef WORLD_DUMMYCOLLECTOR_H
#define WORLD_DUMMYCOLLECTOR_H

#include <world/core/ICollector.h>

template <typename T>
class DummyCollectorChannel : public world::ICollectorChannel<T> {
public:
    void put(
            const ItemKey &key, const T &item,
            const ExplorationContext &ctx = ExplorationContext::getDefault()) override {};

    bool has(const ItemKey &key,
                     const ExplorationContext &ctx =
                     ExplorationContext::getDefault()) const override {

        return false;
    };

    void remove(
            const ItemKey &key,
            const ExplorationContext &ctx = ExplorationContext::getDefault()) override {};
};

#endif //WORLD_DUMMYCOLLECTOR_H
