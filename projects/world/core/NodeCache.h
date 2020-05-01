#ifndef WORLD_NODECACHE_H
#define WORLD_NODECACHE_H

#include "world/core/WorldConfig.h"

#include <string>

#include "world/assets/Image.h"
#include "world/assets/Mesh.h"
#include "WorldKeys.h"

namespace world {

// TODO error management

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

    void saveImage(const std::string &id, const Image &image);

    void saveMesh(const std::string &id, const Mesh &mesh);

    Image readImage(const std::string &id);

    bool readImage(const std::string &id, Image &image);

    Mesh readMesh(const std::string &id);

    void readMeshInplace(const std::string &id, Mesh &mesh);

private:
    NodeCache *_parent = nullptr;

    std::string _directory;
};

} // namespace world

#endif // WORLD_NODECACHE_H
