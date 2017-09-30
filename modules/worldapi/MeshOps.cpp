#include "MeshOps.h"

#include "maths/MathsHelper.h"

using namespace maths;

void MeshOps::recalculateNormals(Mesh & mesh) {
	auto vertList = mesh.getVertices<VType::POSITION>();
	auto faceList = mesh.getFaces();

	std::vector<vec3d> normalSum(vertList.size(), vec3d());
	std::vector<int> normalCount(vertList.size(), 0);

	for (Face & face : faceList) {
		auto faceVertices = face.getIDs<VType::POSITION>();
		int count = face.vertexCount();

		for (int j = 0; j < count; j++) {
			// Calcul de la normale de la face par rapport au point j
			int id0 = faceVertices.at(j);
			int id1 = faceVertices.at(mod(j + 1, count));
			int id2 = faceVertices.at(mod(j - 1, count));

			auto pt1 = vertList.at(id0).getValues();
			auto pt2 = vertList.at(id1).getValues();
			auto pt3 = vertList.at(id2).getValues();

			vec3d v1(pt2[0] - pt1[0], pt2[1] - pt1[1], pt2[2] - pt1[2]);
			vec3d v2(pt3[0] - pt1[0], pt3[1] - pt1[1], pt3[2] - pt1[2]);
			vec3d normal = v1.crossProduct(v2).normalize();

			// Ajout de la normale au calcul de la moyenne
			vec3d & inplace = normalSum.at(id0);
			inplace = inplace + normal;

			normalCount.at(id0)++;

			// Ajout de la normale à la face
			face.setID<VType::NORMAL>(j, id0);
		}
	}

	// Ajout de toutes les normales au mesh
	mesh.clearVertices<VType::NORMAL>();

	for (int i = 0; i < normalSum.size(); i++) {
		int count = normalCount.at(i);
		vec3d normal;

		if (count != 0) {
			normal = normalSum.at(i) / count;
		}
		else {
			normal = vec3d(0, 0, 1);
		}
		
		Vertex<VType::NORMAL> vn;
		vn.add((float) normal.x).add((float) normal.y).add((float) normal.z);
		mesh.addVertex(vn);
	}
}
