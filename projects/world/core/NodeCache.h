#ifndef WORLD_NODECACHE_H
#define WORLD_NODECACHE_H

#include "world/core/WorldConfig.h"

#include <string>

#include "world/assets/Image.h"
#include "world/assets/Mesh.h"
#include "WorldKeys.h"

namespace world {

class Terrain;

// TODO error management

// For cache properties, use a separate struct, so that only the parent
// can be modified.

class WORLDAPI_EXPORT NodeCache {
public:
    NodeCache();
    NodeCache(const std::string &path);
    // TODO const parent ?
    NodeCache(NodeCache &parent, const NodeKey &id);

    void setRoot(const std::string &path);
    void setChild(NodeCache &parent, const NodeKey &id);
    void setSub(NodeCache &parent, const std::string &name);

    bool isRoot() const;
    bool isAvailable() const;
    bool hasChild(const NodeKey &id) const;

    /** Remove everything in the cache. It removes the cache directory
     * as well. */
    void clear();

    /**
     * \brief Create the directory where all the objects from this cache
     * live.
     *
     * This function creates all the required directories, and does
     * nothing if the directory already exists.
     * This function must be called if you don't use the cache methods
     * to save resources (and you save it on your own instead). */
    void createDirectory();

    std::string getPath(const std::string &id) const;

    std::string getDirectory() const;

    std::string getChildDirectory(const NodeKey &key) const;

    /** Save raw data to disk. */
    void saveData(const std::string &id, const void *data, size_t size);

    /** Read data of known size from disk. */
    size_t readData(const std::string &id, void *data, size_t size) const;

    /** Save vector of simple types in raw data form. */
    template <typename T>
    void saveVector(const std::string &id, const std::vector<T> &vec);

    /** Read vector of simple types in raw data form. Add the read elements to
     * vector vec, return the size of read data. */
    template <typename T>
    size_t readVector(const std::string &id, std::vector<T> &vec) const;

    bool hasImage(std::string &id) const;

    void saveImage(const std::string &id, const Image &image);

    Image readImage(const std::string &id) const;

    bool readImage(const std::string &id, Image &image) const;

    void saveMesh(const std::string &id, const Mesh &mesh);

    Mesh readMesh(const std::string &id) const;

    bool readMeshInplace(const std::string &id, Mesh &mesh) const;

    /** Save a terrain to the cache at the specified id. */
    void saveTerrain(const std::string &id, const Terrain &terrain,
                     bool saveTexture = true);

    Terrain readTerrain(const std::string &id, bool readTexture = true) const;

    bool readTerrainInplace(const std::string &id, Terrain &terrain,
                            bool readTexture = true) const;

private:
    NodeCache *_parent = nullptr;

    std::string _directory;
};

template <typename T>
void NodeCache::saveVector(const std::string &id, const std::vector<T> &vec) {
    saveData(id, vec.data(), sizeof(T) * vec.size());
}

template <typename T>
size_t NodeCache::readVector(const std::string &id, std::vector<T> &vec) const {
    if (!isAvailable())
        return 0;
    std::ifstream ifs(getPath(id) + ".bin", std::ios::binary | std::ios::ate);

    if (!ifs.good())
        return 0;
    size_t filesize = static_cast<size_t>(ifs.tellg());

    if (filesize % sizeof(T) != 0) {
        throw std::runtime_error("Size does not match: data of size " + std::to_string(filesize)
            + " cannot be a vector of a type of size " + std::to_string(sizeof(T)));
    }
    
    size_t vecsize = filesize / sizeof(T);
    vec.resize(vecsize);

    ifs.seekg(0, ifs.beg);
    ifs.read(reinterpret_cast<char*>(vec.data()), filesize);
    ifs.close();

    return vecsize;
}

} // namespace world

#endif // WORLD_NODECACHE_H
