#pragma once

#include "core/WorldConfig.h"

#include <memory>
#include <vector>

#include "core/WorldTypes.h"
#include "Object3D.h"
#include "Material.h"
#include "Image.h"

namespace world {

	class PrivateScene;

	class WORLDAPI_EXPORT Scene {
	public:
		Scene();

		virtual ~Scene();

		/** Add all the content from the other scene in this scene. */
		void addAll(const Scene &other);

		template<typename... Args>
		Object3D &createObject(Args... args) {
			Object3D *object = new Object3D(args...);
			addObjectInternal(object);
			return *object;
		}

		void addObject(const Object3D &object);

		void getObjects(std::vector<Object3D *> &output) const;

		std::vector<Object3D *> getObjects() const;

		void addMaterial(const Material &material);

		void addMaterial(const std::shared_ptr<Material> &material);

		/** Insert all the materials from this scene in the output vector.*/
		void getMaterials(std::vector<std::shared_ptr<Material>> &output) const;

		std::vector<std::shared_ptr<Material>> getMaterials() const;

		void addTexture(const std::string &id, const Image &image);

		optional<const Image&> getTexture(const std::string &id) const;

	private:
		void addObjectInternal(Object3D *object);

		PrivateScene *_internal;
	};
}
