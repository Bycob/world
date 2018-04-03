#pragma once

#include "core/WorldConfig.h"

#include <memory>
#include <vector>

#include "Object3D.h"
#include "Material.h"

namespace world {

	class PrivateScene;

	class WORLDAPI_EXPORT Scene {
	public:
		Scene();

		virtual ~Scene();

		/** Ajoute tout le contenu de la sc�ne pass�e en param�tres, dans cette
        sc�ne. */
		void addAll(const Scene &other);

		template<typename... Args>
		Object3D &createObject(Args... args) {
			Object3D *object = new Object3D(args...);
			addObjectInternal(object);
			return *object;
		}

		void addMaterial(const std::shared_ptr<Material> &material);

		void getObjects(std::vector<Object3D *> &output) const;

		std::vector<Object3D *> getObjects() const;

		/** Insert tous les mat�riaux contenus dans cette sc�ne � la fin du vecteur
        "output".*/
		void getMaterials(std::vector<std::shared_ptr<Material>> &output) const;

		std::vector<std::shared_ptr<Material>> getMaterials() const;

	private:
		void addObjectInternal(Object3D *object);

		PrivateScene *_internal;
	};
}
