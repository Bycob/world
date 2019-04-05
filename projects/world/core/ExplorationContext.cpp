#include "ExplorationContext.h"

namespace world {

const ExplorationContext &ExplorationContext::getDefault() {
    static ExplorationContext defaultCtx;
    return defaultCtx;
}

ExplorationContext::ExplorationContext()
        : _keyPrefix(ItemKeys::defaultKey()), _offset(0, 0, 0) {}

void ExplorationContext::addOffset(const vec3d &offset) { _offset += offset; }

void ExplorationContext::appendPrefix(const NodeKey &prefix) {
    _keyPrefix = ItemKeys::child(_keyPrefix, prefix);
}

ItemKey ExplorationContext::mutateKey(const ItemKey &key) const {
    return ItemKeys::concat(_keyPrefix, key);
}

vec3d ExplorationContext::getOffset() const { return _offset; }
} // namespace world
