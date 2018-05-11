#ifndef WORLD_EXPLORATIONRESULT_H
#define WORLD_EXPLORATIONRESULT_H

#include "core/WorldConfig.h"

#include <list>

#include "WorldZone.h"

namespace world {

class WORLDAPI_EXPORT ExplorationResult {
public:
    class Iterator;

    ExplorationResult();

    void appendZone(const WorldZone &zone);

    Iterator begin();

    Iterator end();

private:
    std::list<WorldZone> _zones;
};

class WORLDAPI_EXPORT ExplorationResult::Iterator {
public:
    Iterator operator++();

    bool operator==(const Iterator &other);

    bool operator!=(const Iterator &other);

    WorldZone &operator*();

private:
    std::list<WorldZone>::iterator _it;

    Iterator(std::list<WorldZone>::iterator it);

    friend class ExplorationResult;
};
} // namespace world

#endif // WORLD_EXPLORATIONRESULT_H
