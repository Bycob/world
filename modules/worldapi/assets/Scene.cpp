#include "Scene.h"

#include <map>

namespace world {

class PScene {
public:
    std::vector<std::unique_ptr<Object3D>> _objects;
    std::vector<std::shared_ptr<Material>> _materials;
    std::map<std::string, Image> _images;
};

Scene::Scene() : _internal(new PScene()) {}

Scene::~Scene() { delete _internal; }

void Scene::addAll(const Scene &other) {
    for (const std::unique_ptr<Object3D> &object : other._internal->_objects) {
        _internal->_objects.emplace_back(new Object3D(*object));
    }

    for (const std::shared_ptr<Material> &material :
         other._internal->_materials) {
        _internal->_materials.push_back(material);
    }
}

void world::Scene::addObject(const Object3D &object) {
    addObjectInternal(new Object3D(object));
}

void Scene::getObjects(std::vector<Object3D *> &output) const {
    for (const std::unique_ptr<Object3D> &object : _internal->_objects) {
        output.push_back(object.get());
    }
}

std::vector<Object3D *> Scene::getObjects() const {
    std::vector<Object3D *> output;
    getObjects(output);
    return output;
}

void world::Scene::addMaterial(const Material &material) {
    addMaterial(std::make_shared<Material>(material));
}

void Scene::addMaterial(const std::shared_ptr<Material> &material) {
    _internal->_materials.push_back(material);
}

void Scene::getMaterials(std::vector<std::shared_ptr<Material>> &output) const {
    for (const std::shared_ptr<Material> &material : _internal->_materials)
        output.push_back(material);
}

std::vector<std::shared_ptr<Material>> Scene::getMaterials() const {
    std::vector<std::shared_ptr<Material>> output;
    getMaterials(output);
    return output;
}

void Scene::addTexture(const std::string &id, const Image &image) {
    _internal->_images.emplace(id, image);
}

optional<const Image &> Scene::getTexture(const std::string &id) const {
    auto it = _internal->_images.find(id);

    if (it != _internal->_images.end()) {
        return it->second;
    } else {
        return nullopt;
    }
}

void Scene::addObjectInternal(Object3D *object) {
    _internal->_objects.emplace_back(object);
}
} // namespace world
