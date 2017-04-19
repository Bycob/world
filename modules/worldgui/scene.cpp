#include "scene.h"

#include <worldapi/ObjLoader.h>

Scene::Scene()
{

}

void Scene::addMesh(std::shared_ptr<Mesh> &mesh)
{
    meshes.push_back(mesh);
}

void Scene::addMaterial(std::shared_ptr<Material> &material)
{
    materials.push_back(material);
}

const std::vector<std::shared_ptr<Mesh> > &Scene::getMeshes() const
{
    return meshes;
}

const std::vector<std::shared_ptr<Material> > &Scene::getMaterials() const
{
    return materials;
}

void Scene::save(QString path) const {
    ObjLoader file;

    for (auto & mesh : meshes) {
        file.addMesh(mesh);
    }

    for (auto & material : materials) {
        file.addMaterial(material);
    }

    file.write(path.toStdString());
}
