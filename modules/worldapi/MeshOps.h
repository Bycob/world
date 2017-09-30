#pragma once

#include "worldapidef.h"

#include "Mesh.h"

class WORLDAPI_EXPORT MeshOps {
public :
	MeshOps() = delete;

	/** Calcule la normale points par points du mesh passé en paramètre.
	Un vertex est associé à une normale */
	static void recalculateNormals(Mesh & mesh);
};