#include "ExplorationResult.h"

namespace world {

// ===== EXPLORATION RESULT

ExplorationResult::ExplorationResult() {}

void ExplorationResult::appendZone(const WorldZone &zone) {
    _zones.push_back(zone);
}

ExplorationResult::Iterator ExplorationResult::begin() {
    return Iterator(_zones.begin());
}

ExplorationResult::Iterator ExplorationResult::end() {
    return Iterator(_zones.end());
}


// ===== ITERATOR

ExplorationResult::Iterator::Iterator(std::list<WorldZone>::iterator it)
        : _it(it) {}

ExplorationResult::Iterator ExplorationResult::Iterator::operator++() {
    ++_it;
    return *this;
}

bool ExplorationResult::Iterator::operator==(const Iterator &other) {
    return _it == other._it;
}

bool ExplorationResult::Iterator::operator!=(const Iterator &other) {
    return _it != other._it;
}

WorldZone &ExplorationResult::Iterator::operator*() { return *_it; }
} // namespace world