#include "GridStorageReducer.h"

#include <set>

#include "GridStorage.h"

namespace world {

void GridStorageReducer::setMaxInstances(u32 maxInstances) {
    _maxInstances = maxInstances;
}

void GridStorageReducer::registerStorage(GridStorageBase *storage) {
    if (std::find(_storages.begin(), _storages.end(), storage) ==
        _storages.end()) {
        _storages.push_back(storage);
    }
}

void GridStorageReducer::registerAccess(const TileCoordinates &tc) {
    _accessTracker[tc] = _accessCounter;
    ++_accessCounter;
}

void GridStorageReducer::reduceStorage() {
    size_t currentSize = _accessTracker.size();

    if (currentSize < _maxInstances) {
        return;
    }

    typedef std::pair<u64, TileCoordinates> Access;

    // Add all access to accesses
    std::set<TileCoordinates> processedChildren;

    for (const auto &e : _accessTracker) {
        processedChildren.insert(e.first);
    }

    while (!processedChildren.empty()) {
        std::set<TileCoordinates> next;

        for (const auto &tc : processedChildren) {
            const TileCoordinates &parent =
                _tileSystem.getParentTileCoordinates(tc);

            if (parent._lod >= 0) {
                u64 childAccess = _accessTracker[tc];
                u64 parentAccess = _accessTracker[parent];

                if (parentAccess < childAccess) {
                    _accessTracker[parent] = childAccess;
                    next.insert(parent);
                }
            }
        }
        processedChildren = next;
    }

    // Sort all accesses by ascending access counter. If tie, child (== highest
    // lod) goes first (it will be the first to be deleted)
    std::vector<Access> accesses;

    for (const auto &e : _accessTracker) {
        accesses.emplace_back(e.second, e.first);
    }

    std::sort(accesses.begin(), accesses.end(),
              [](const Access &lhs, const Access &rhs) {
                  return lhs.first != rhs.first
                             ? lhs.first < rhs.first
                             : lhs.second._lod > rhs.second._lod;
              });

    // Remove old accesses
    u64 removeCount = currentSize - _maxInstances;

    for (auto storage : _storages) {
        for (u64 i = 0; i < removeCount; ++i) {
            storage->remove(accesses[i].second, true);
        }
    }

    for (u64 i = 0; i < removeCount; ++i) {
        _accessTracker.erase(accesses[i].second);
    }
}
} // namespace world
