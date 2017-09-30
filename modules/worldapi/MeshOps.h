#pragma once

#include "worldapidef.h"

#include "Mesh.h"

class WORLDAPI_EXPORT MeshOps {
public :
	MeshOps() = delete;

	/** Calcule la normale points par points du mesh pass� en param�tre.
	Un vertex est associ� � une normale */
	static void recalculateNormals(Mesh & mesh);
};