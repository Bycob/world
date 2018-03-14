#pragma once

#include <memory>

#include <worldapi/worldapidef.h>

#include "../world/WorldObject.h"
#include "../Mesh.h"
#include "../Material.h"
#include "TreeSkeletton.h"

class WORLDAPI_EXPORT Tree : public WorldObject {
public:
	Tree();
	virtual ~Tree();

	const Mesh & getTrunkMesh() {
		return _trunkMesh;
	}

	void collectWholeObject(WorldZone &zone, CollectorObject &collector) override;
private :
	friend class TreeGenerator;

	Mesh _trunkMesh;
	Material _trunkMaterial;
};

