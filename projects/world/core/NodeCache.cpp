#include "NodeCache.h"

#include "IOUtil.h"

namespace world {

NodeCache::NodeCache() : _directory("") {}

NodeCache::NodeCache(const std::string &path) : _directory(path) {}

NodeCache::NodeCache(NodeCache &parent, const NodeKey &id) {
    setChild(parent, id);
}

void NodeCache::setChild(NodeCache &parent, const NodeKey &id) {
    _parent = &parent;
    _directory = NodeKeys::toString(id) + "/";
}

void NodeCache::setSub(NodeCache &parent, const std::string &name) {
    _parent = &parent;
    _directory = name;
}

void NodeCache::setRoot(const std::string &path) {
    _parent = nullptr;
    _directory = path;
}

bool NodeCache::isRoot() const { return _parent == nullptr; }

bool NodeCache::isAvailable() const {
    if (_parent != nullptr) {
        return _parent->isAvailable();
    } else {
        return !_directory.empty();
    }
}

void NodeCache::createDirectory() {
    if (!isAvailable()) {
        throw std::runtime_error("Cache is not available!");
    }

    createDirectories(getDirectory());
}

std::string NodeCache::getPath(const std::string &id) const {
    if (!isAvailable()) {
        throw std::runtime_error("Cache is not available!");
    }
    return getDirectory() + id;
}

std::string NodeCache::getDirectory() const {
    if (_parent == nullptr) {
        return _directory;
    } else {
        return _parent->getDirectory() + _directory;
    }
}

std::string NodeCache::getChildDirectory(const NodeKey &key) const {
    return getDirectory() + NodeKeys::toString(key) + "/";
}

void NodeCache::saveImage(const std::string &id, const Image &image) {
    if (!isAvailable())
        return;
    createDirectory();
    image.write(getPath(id) + ".png");
}

Image NodeCache::readImage(const std::string &id) {
    createDirectory();
    return Image::read(getPath(id) + ".png");
}

bool NodeCache::readImage(const std::string &id, Image &image) {
    try {
        image = readImage(id);
        return true;
    } catch (std::runtime_error &e) {
        return false;
    }
}

} // namespace world
