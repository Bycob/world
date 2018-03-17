#include "Scene.h"

namespace world {

	class PrivateScene {
	public:
		std::vector<std::unique_ptr<Object3D>> _objects;
		std::vector<std::shared_ptr<Material>> _materials;
	};

	Scene::Scene() : _internal(new PrivateScene()) {

	}

	Scene::~Scene() {
		delete _internal;
	}

	void Scene::addAll(const Scene &other) {
		for (const std::unique_ptr<Object3D> &object : other._internal->_objects) {
			_internal->_objects.emplace_back(new Object3D(*object));
		}

		for (const std::shared_ptr<Material> &material : other._internal->_materials) {
			_internal->_materials.push_back(material);
		}
	}

	void Scene::addMaterial(const std::shared_ptr<Material> &material) {
		_internal->_materials.push_back(material);
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

	void Scene::getMaterials(std::vector<std::shared_ptr<Material>> &output) const {
		for (const std::shared_ptr<Material> &material : _internal->_materials)
			output.push_back(material);
	}

	std::vector<std::shared_ptr<Material>> Scene::getMaterials() const {
		std::vector<std::shared_ptr<Material>> output;
		getMaterials(output);
		return output;
	}

	void Scene::addObjectInternal(Object3D *object) {
		_internal->_objects.emplace_back(object);
	}
}
