#include "Object3D.h"

namespace world {

	Object3D::Object3D()
			: Object3D(Mesh()) {

	}

	Object3D::Object3D(const Mesh &mesh)
			: _position(0, 0, 0), _scale(1, 1, 1),
			  _mesh(mesh) {

	}

	Object3D::Object3D(world::Mesh &mesh, bool keepRef)
			: _position(0, 0, 0), _scale(1, 1, 1),
			  _mesh(mesh, keepRef) {

	}

	Object3D::~Object3D() {

	}

	void Object3D::setMesh(const Mesh &mesh) {
		_mesh = RefOrValue<Mesh>(mesh);
	}

	void Object3D::setMesh(Mesh &mesh, bool keepRef) {
		_mesh = RefOrValue<Mesh>(mesh, keepRef);
	}

	void Object3D::setPosition(const vec3d &position) {
		_position = position;
	}

	void Object3D::setScale(const vec3d &scale) {
		_scale = scale;
	}

	void Object3D::setMaterialID(const std::string & materialID) {
		_materialID = materialID;
	}

	std::string Object3D::getMaterialID() const {
		return _materialID;
	}
}
