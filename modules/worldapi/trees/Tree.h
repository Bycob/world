#pragma once

#include "../worldapidef.h"
#include "../mesh.h"
#include "../Material.h"
#include "TreeSkeletton.h"

#include <memory>

class WORLDAPI_EXPORT Tree {
public:
	Tree();
	~Tree();

	const Mesh & getTrunkMesh() {
		return _trunkMesh;
	}
private :
	friend class TreeGenerator;

	Mesh _trunkMesh;
	Material _trunkMaterial;
};

