#include "IInstanceGenerator.h"

namespace world {

Template::Template(const SceneNode &node) { insert(node); }

void Template::insert(const Item &item) {
    if (item._minRes < 0)
        throw std::runtime_error("Min res cannot be under 0.");

    auto it = _items.begin();

    while (it != _items.end() && it->_minRes > item._minRes) {
        ++it;
    }

    _items.insert(it, item);
}

void Template::insert(double resolution, const SceneNode &node) {
    insert(Item{{node}, resolution});
}

/*Item &getOrCreate(double resolution) {
    auto *item = getAt(resolution);
    if (item != nullptr) {
        return *item;
    }
    else {
        insert(Item{{}, resolution});
    }
}*/

void Template::insert(const SceneNode &node) {
    Item *zero = getAt(0);
    if (zero != nullptr) {
        zero->_nodes.push_back(node);
    } else {
        insert({{node}, 0});
    }
}

Template::Item *Template::getAt(double resolution) {
    for (Item &item : _items) {
        if (item._minRes <= resolution)
            return &item;
    }
    return nullptr;
}

SceneNode Template::getDefaultNode() {
    auto *u = getAt(0);
    if (u != nullptr && !u->_nodes.empty()) {
        return u->_nodes.at(0);
    } else {
        return SceneNode();
    }
}
} // namespace world
