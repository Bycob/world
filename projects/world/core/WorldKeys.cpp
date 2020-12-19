#include "WorldKeys.h"

#include "WorldTypes.h"

namespace world {

NodeKey::NodeKey(int id) : _name(std::to_string(id)) {}

bool NodeKey::valid() const {
    for (char c : _name) {
        bool legal = (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
                     (c >= '1' && c <= '9') || c == '0' || c == '_' ||
                     c == '.' || c == '-';
        if (!legal)
            return false;
    }
    return true;
}

bool NodeKey::operator==(const NodeKey &other) const {
    return _name == other._name;
}

bool NodeKey::operator!=(const NodeKey &other) const {
    return _name != other._name;
}

bool NodeKey::operator<(const NodeKey &other) const {
    return _name < other._name;
}

bool NodeKey::operator>(const NodeKey &other) const {
    return _name > other._name;
}

std::string ItemKey::str() const {
    std::string result;
    for (size_t i = 0; i < _components.size(); ++i) {
        result += i == 0 ? "" : KEY_SEPARATOR_STR;
        result += _components[i];
    }
    return result;
}

ItemKey ItemKey::parent() const {
    if (_components.empty()) {
        throw std::runtime_error("no parent");
    }

    ItemKey result = *this;
    result._components.pop_back();
    return result;
}


NodeKey NodeKeys::fromUint(unsigned int id) { return std::to_string(id); }

NodeKey NodeKeys::fromInt(int id) { return std::to_string(id); }

std::string NodeKeys::toString(const world::NodeKey &key) { return key._name; }

NodeKey NodeKeys::fromString(const std::string &str) { return str; }

ItemKey ItemKeys::fromString(const std::string &str) {
    ItemKey result;
    size_t sep;
    size_t start = 0;

    do {
        sep = str.find_first_of('/', start);
        std::string keystr;

        if (sep == std::string::npos) {
            keystr = str.substr(start);
        } else {
            keystr = str.substr(start, sep - start);
            start = sep + 1;
        }

        try {
            result._components.push_back(NodeKeys::fromString(keystr));
        } catch (std::invalid_argument &e) {
            throw e;
        }
    } while (sep != std::string::npos);

    return result;
}

} // namespace world