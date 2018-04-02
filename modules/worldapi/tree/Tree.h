#pragma once

#include "core/WorldConfig.h"

#include <memory>

#include "core/WorldObject.h"
#include "assets/Mesh.h"
#include "assets/Material.h"
#include "TreeSkeletton.h"

namespace world {

	class WORLDAPI_EXPORT Tree : public WorldObject {
	public:
		Tree();

		virtual ~Tree();

		const Mesh &getTrunkMesh() {
			return _trunkMesh;
		}

		void collectWholeObject(ICollector &collector) override;

	private :
		friend class TreeGenerator;

		Mesh _trunkMesh;
		Material _trunkMaterial;
	};
}
