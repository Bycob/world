#ifndef WORLD_OBJECTNODE_H
#define WORLD_OBJECTNODE_H

#include "WorldConfig.h"

#include "WorldNode.h"
#include "IInstanceGenerator.h"

namespace world {

class WORLDAPI_EXPORT ObjectLod {
public:
    u32 _id;
    double _resolution;


    ObjectLod(u32 id, double resolution, size_t meshCount = 0);

    std::string getId(size_t i) const;

    /** Add meshes to the lod */
    void addMeshes(size_t count);

    Mesh &addMesh();

    Mesh &getMesh(size_t i);

    size_t getMeshCount() const { return _meshes.size(); }

    SceneNode &getNode(size_t i);

    size_t getNodeCount() const { return _nodes.size(); }

    void addMaterials(size_t count);

    Material &addMaterial();

    Material &getMaterial(size_t i);

    size_t getMaterialCount() const { return _materials.size(); }

    Image &addTexture(int width = 1, int height = 1,
                      ImageType imgType = ImageType::RGB);

    void addTextures(size_t count);

    Image &getTexture(size_t i);

    size_t getTextureCount() const { return _textures.size(); }

    void collect(Template &tp, ICollector &collector,
                 const ExplorationContext &ctx) const;

private:
    // Meshes, textures, and materials can be reused from
    // other objectLod or even an unrelated source
    std::vector<SceneNode> _nodes;
    std::vector<Mesh> _meshes;
    std::vector<Material> _materials;
    std::vector<Image> _textures;
};

class WORLDAPI_EXPORT ObjectInstance {
public:
    ObjectInstance(const vec3d &pos = {});

    void setPosition(const vec3d &pos) { _pos = pos; }

    vec3d getPosition() const {return _pos;}

    ObjectLod &addLod(double resolution, size_t meshCount = 0);

    ObjectLod &getLod(size_t i);

    int getLodCount() const { return _lods.size(); }

    ObjectLod &getLodByResolution(double res);

    Template collect(ICollector &collector, const ExplorationContext &ctx,
                     double maxRes) const;

private:
    std::vector<ObjectLod> _lods;

    vec3d _pos;
};

/** This node represents one object, like a tree or a rock.
 * It contains all the meshes / materials / textures at multiple
 * resolutions. */
class WORLDAPI_EXPORT ObjectNode : public WorldNode {
public:
    // TODO
};
} // namespace world

#endif // WORLD_OBJECTNODE_H
