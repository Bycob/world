#pragma once

#include "world/core/WorldConfig.h"

#include <memory>
#include <vector>

#include "world/core/WorldTypes.h"
#include "SceneNode.h"
#include "Material.h"
#include "Image.h"

namespace world {

class PScene;

class WORLDAPI_EXPORT Scene {
public:
    Scene();

    ~Scene();

    Scene(const Scene &other);

    Scene(Scene &&other);

    Scene &operator=(const Scene &other);

    Scene &operator=(Scene &&other);

    void clear();

    /** Add all the content from the other scene in this scene. */
    void addAll(const Scene &other);

    void addNode(const SceneNode &object);

    /** Add the node, connect it to the mesh and create mesh name if it is
     * empty. */
    void addMeshNode(const SceneNode &node, const Mesh &mesh);

    std::vector<SceneNode *> getNodes() const;

    void addMesh(std::string id, const Mesh &mesh);

    void addMesh(const Mesh &mesh);

    bool hasMesh(const std::string &id) const;

    u32 meshCount() const;

    const Mesh &getMesh(const std::string &id) const;

    void addMaterial(std::string id, const Material &material);

    void addMaterial(const Material &material);

    bool hasMaterial(const std::string &id) const;

    u32 materialCount() const;

    const Material &getMaterial(const std::string &id) const;

    void addTexture(std::string id, const Image &image);

    bool hasTexture(const std::string &id) const;

    const Image &getTexture(const std::string &id) const;

private:
    PScene *_internal;


    std::string newMeshName();

    std::string newMaterialName();
};
} // namespace world
