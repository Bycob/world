#include <tinydir/tinydir.h>
#include "NodeCache.h"

#include "IOUtil.h"
#include "world/terrain/Terrain.h"

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

bool NodeCache::hasChild(const NodeKey &id) const {
    if (!isAvailable()) {
        return false;
    }
    // TODO tests & benchmark
    tinydir_dir dir;
    bool exists = tinydir_open(&dir, getPath(id).c_str()) != -1;
    tinydir_close(&dir);
    return exists;
}

void NodeCache::clear() { removeDirectory(getDirectory()); }

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

void NodeCache::saveData(const std::string &id, const void *data, size_t size) {
    if (!isAvailable())
        return;
    createDirectory();

    std::ofstream ofs(getPath(id) + ".bin", std::ios::binary);
    ofs.write(reinterpret_cast<const char *>(data), size);
}

size_t NodeCache::readData(const std::string &id, void *data,
                           size_t size) const {
    std::ifstream ifs(getPath(id) + ".bin", std::ios::binary);
    ifs.read(reinterpret_cast<char *>(data), size);
    if (ifs.fail()) {
        return 0;
    }
    return ifs.gcount();
}

void NodeCache::saveImage(const std::string &id, const Image &image) {
    if (!isAvailable())
        return;
    createDirectory();
    image.write(getPath(id) + ".png");
}

Image NodeCache::readImage(const std::string &id) const {
    return Image::read(getPath(id) + ".png");
}

bool NodeCache::readImage(const std::string &id, Image &image) const {
    try {
        image = readImage(id);
        return true;
    } catch (std::runtime_error &e) {
        return false;
    }
}

void NodeCache::saveMesh(const std::string &id, const Mesh &mesh) {
    if (!isAvailable())
        return;
    createDirectory();
    // TODO write mesh to disk
}

Mesh NodeCache::readMesh(const std::string &id) const {
    throw std::runtime_error("NodeCache::readMesh not implemented");
}

bool NodeCache::readMeshInplace(const std::string &id, Mesh &mesh) const {
    throw std::runtime_error("NodeCache::readMesh not implemented");
}

void NodeCache::saveTerrain(const std::string &id, const Terrain &terrain,
                            bool saveTexture) {
    if (!isAvailable())
        return;
    createDirectory();

    // write raw to disk
    std::ofstream ofstr(getPath(id) + ".raw", std::ios::binary);
    int res = terrain.getResolution();

    for (int x = 0; x < res; ++x) {
        for (int y = 0; y < res; ++y) {
            f32 v = terrain(x, y);
            ofstr.write(reinterpret_cast<char *>(&v), sizeof(v));
        }
    }

    // save texture to disk
    if (saveTexture)
        terrain.getTexture().write(getPath(id) + ".png");
}

Terrain NodeCache::readTerrain(const std::string &id, bool readTexture) const {
    std::ifstream ifstr(getPath(id) + ".raw", std::ios::binary);

    if (ifstr.fail()) {
        throw std::runtime_error("The file " + getPath(id) + " does not exist");
    }

    ifstr.seekg(0, std::ios::end);
    size_t fileSize = ifstr.tellg() / sizeof(f32);
    int res = int(sqrt(fileSize));

    if (res * res != fileSize) {
        throw std::runtime_error("The file " + getPath(id) +
                                 ".raw does not contain a terrain");
    }

    ifstr.seekg(0, std::ios::beg);
    Terrain terrain(res);

    for (int x = 0; x < res; ++x) {
        for (int y = 0; y < res; ++y) {
            f32 v;
            ifstr.read(reinterpret_cast<char *>(&v), sizeof(v));
            terrain(x, y) = v;
        }
    }

    // Read terrain texture
    if (readTexture)
        terrain.setTexture(Image::read(getPath(id) + ".png"));
    return terrain;
}

bool NodeCache::readTerrainInplace(const std::string &id, Terrain &terrain,
                                   bool readTexture) const {
    try {
        terrain = readTerrain(id, readTexture);
        return true;
    } catch (std::runtime_error &e) {
        // std::cerr << e.what() << std::endl;
        return false;
    }
}

} // namespace world
