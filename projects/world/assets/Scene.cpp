#include "Scene.h"

#include <map>

namespace world {

// We use shared ptr because the big resources like meshes or
// images may be shared in the future
typedef std::shared_ptr<Mesh> SharedMesh;
typedef std::shared_ptr<Image> SharedImage;

class PScene {
public:
    std::vector<std::unique_ptr<SceneNode>> _nodes;
    std::map<std::string, std::unique_ptr<Material>> _materials;
    std::map<std::string, SharedMesh> _meshes;
    std::map<std::string, SharedImage> _images;

    u32 _meshCounter = 0;
    u32 _materialCounter = 0;
};

Scene::Scene() : _internal(new PScene()) {}

Scene::~Scene() { delete _internal; }

Scene::Scene(const Scene &other) : Scene() { addAll(other); }

Scene::Scene(Scene &&other) : _internal(other._internal) {
    other._internal = nullptr;
}

Scene &Scene::operator=(const Scene &other) {
    clear();
    addAll(other);
}

Scene &Scene::operator=(Scene &&other) {
    delete _internal;
    _internal = other._internal;
    other._internal = nullptr;
}

void Scene::clear() {
    _internal->_nodes.clear();
    _internal->_materials.clear();
    _internal->_meshes.clear();
    _internal->_images.clear();
}

void Scene::addAll(const Scene &other) {
    // Deep copy
    for (const auto &object : other._internal->_nodes) {
        _internal->_nodes.emplace_back(std::make_unique<SceneNode>(*object));
    }

    for (const auto &item : other._internal->_materials) {
        _internal->_materials[item.first] =
            std::make_unique<Material>(*item.second);
    }

    for (const auto &item : other._internal->_meshes) {
        _internal->_meshes[item.first] = std::make_shared<Mesh>(*item.second);
    }

    for (const auto &item : other._internal->_images) {
        _internal->_images[item.first] = std::make_shared<Image>(*item.second);
    }
}

void Scene::addNode(const SceneNode &object) {
    _internal->_nodes.push_back(std::make_unique<SceneNode>(object));
}

// TODO faire des tests sur ça
void Scene::addMeshNode(const SceneNode &node, const Mesh &mesh) {
    std::string meshName = mesh.getName();

    if (meshName.empty()) {
        meshName = newMeshName();
    }

    addMesh(meshName, mesh);
    addNode(node);
    _internal->_nodes.back()->setMesh(meshName);
}

std::vector<SceneNode *> Scene::getNodes() const {
    std::vector<SceneNode *> output;
    for (const std::unique_ptr<SceneNode> &object : _internal->_nodes) {
        output.push_back(object.get());
    }
    return output;
}

void Scene::addMesh(std::string id, const Mesh &mesh) {
    _internal->_meshes[id] = std::make_shared<Mesh>(mesh);
}

void Scene::addMesh(const Mesh &mesh) {
    if (mesh.getName().empty()) {
        // If we make up a name for the mesh, user will not be able to
        // reference it anyway
        throw std::runtime_error("mesh name is empty");
    }
    addMesh(mesh.getName(), mesh);
}

bool Scene::hasMesh(const std::string &id) const {
    return _internal->_meshes.find(id) != _internal->_meshes.end();
}

u32 Scene::meshCount() const { return _internal->_meshes.size(); }

const Mesh &Scene::getMesh(const std::string &id) const {
    return *_internal->_meshes.at(id);
}

void Scene::addMaterial(std::string id, const Material &material) {
    _internal->_materials[id] = std::make_unique<Material>(material);
}

void Scene::addMaterial(const Material &material) {
    if (material.getName().empty()) {
        // If we make up a name for the mesh, user will not be able to
        // reference it anyway
        throw std::runtime_error("material name is empty");
    }

    addMaterial(material.getName(), material);
}

bool Scene::hasMaterial(const std::string &id) const {
    return _internal->_materials.find(id) != _internal->_materials.end();
}

u32 Scene::materialCount() const { return _internal->_materials.size(); }

const Material &Scene::getMaterial(const std::string &id) const {
    return *_internal->_materials.at(id);
}

void Scene::addTexture(std::string id, const Image &image) {
    _internal->_images.emplace(id, std::make_shared<Image>(image));
}

bool Scene::hasTexture(const std::string &id) const {
    return _internal->_images.find(id) != _internal->_images.end();
}

const Image &Scene::getTexture(const std::string &id) const {
    return *_internal->_images.at(id);
}

std::string Scene::newMeshName() {
    std::string newName;
    do {
        newName = "mesh" + std::to_string(_internal->_meshCounter);
        _internal->_meshCounter++;
    } while (_internal->_meshes.find(newName) != _internal->_meshes.end());
    return newName;
}

std::string Scene::newMaterialName() {
    std::string newName;
    do {
        newName = "mat" + std::to_string(_internal->_materialCounter);
        _internal->_materialCounter++;
    } while (hasMaterial(newName));
    return newName;
}

} // namespace world
