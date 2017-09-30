#pragma once

#include <worldapi/worldapidef.h>

#include "../world/WorldObject.h"
#include "../Mesh.h"
#include "../Material.h"
#include "TreeSkeletton.h"

#include <memory>

class WORLDAPI_EXPORT Tree : public WorldObject {
public:
	Tree();
	virtual ~Tree();

	const Mesh & getTrunkMesh() {
		return _trunkMesh;
	}

	void fillScene(Scene & scene) const override;
private :
	friend class TreeGenerator;

	Mesh _trunkMesh;
	Material _trunkMaterial;
};

