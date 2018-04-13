#pragma once

#include "core/WorldConfig.h"

#include <memory>

#include "core/Memory.h"
#include "math/MathsHelper.h"
#include "Mesh.h"

namespace world {

	class WORLDAPI_EXPORT Object3D {
	public:
		Object3D();

		Object3D(const Mesh &mesh);

		virtual ~Object3D();

		void setMesh(const Mesh &mesh);

		const Mesh &getMesh() const { return *_mesh; }

		void setPosition(const vec3d &position);

		const vec3d &getPosition() const { return _position; }

		void setScale(const vec3d &scale);

		const vec3d &getScale() const { return _scale; }

		void setMaterialID(const std::string &materialID);

		std::string getMaterialID() const;

	private:
		// Object3D is only a user of the mesh. Thus it can share a
		// permanent reference to the mesh (ie, that doesn't disappear)
		std::shared_ptr<Mesh> _mesh;

		std::string _materialID;
		vec3d _position;
		vec3d _scale;
	};
}
