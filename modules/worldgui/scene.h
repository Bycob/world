#ifndef RENDERABLE3DOBJECTS_H
#define RENDERABLE3DOBJECTS_H

#include <memory>
#include <worldapi/mesh.h>
#include <worldapi/Material.h>

class Scene
{
public:
    Scene();

    void addMesh(std::shared_ptr<Mesh> & mesh);
    void addMaterial(std::shared_ptr<Material> & material);

    const std::vector<std::shared_ptr<Mesh>> & getMeshes() const;
    const std::vector<std::shared_ptr<Material>> & getMaterials() const;
private:
    std::vector<std::shared_ptr<Mesh>> meshes;
    std::vector<std::shared_ptr<Material>> materials;
};

#endif // RENDERABLE3DOBJECTS_H
