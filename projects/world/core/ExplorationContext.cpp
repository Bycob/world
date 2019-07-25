#include "ExplorationContext.h"

namespace world {

const ExplorationContext &ExplorationContext::getDefault() {
    static ExplorationContext defaultCtx;
    return defaultCtx;
}

ExplorationContext::ExplorationContext()
        : _keyPrefix(ItemKeys::defaultKey()),
          _offset(0, 0, 0), _environment{nullptr} {}

void ExplorationContext::addOffset(const vec3d &offset) { _offset += offset; }

void ExplorationContext::appendPrefix(const NodeKey &prefix) {
    _keyPrefix = {_keyPrefix, prefix};
}

void ExplorationContext::setEnvironment(IEnvironment *environment) {
    _environment = environment;
}

ItemKey ExplorationContext::mutateKey(const ItemKey &key) const {
    return {_keyPrefix, key};
}

ItemKey ExplorationContext::operator()(const ItemKey &key) const {
    return mutateKey(key);
}

SceneNode ExplorationContext::createNode(const ItemKey &meshKey,
                                         const ItemKey &materialKey) const {
    std::string meshID =
        meshKey == ItemKeys::defaultKey() ? "" : mutateKey(meshKey).str();
    std::string materialID = materialKey == ItemKeys::defaultKey()
                                 ? ""
                                 : mutateKey(materialKey).str();
    return SceneNode(meshID, materialID);
}

vec3d ExplorationContext::getOffset() const { return _offset; }

bool ExplorationContext::hasEnvironment() const {
    return _environment != nullptr;
}

const IEnvironment &ExplorationContext::getEnvironment() const {
    if (!hasEnvironment()) {
        throw std::runtime_error("No environment");
    }
    return *_environment;
}

} // namespace world
