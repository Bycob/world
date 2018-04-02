#include "MeshOps.h"

#include "math/MathsHelper.h"

namespace world {

	void MeshOps::recalculateNormals(Mesh &mesh) {
		auto &vertList = mesh.getVertices();
		auto &faceList = mesh.getFaces();

		std::vector<vec3d> normalSum(vertList.size(), vec3d());
		std::vector<int> normalCount(vertList.size(), 0);

		for (const Face &face : faceList) {
			const int count = face.vertexCount();

			for (int j = 0; j < count; j++) {
				// Calcul de la normale de la face par rapport au point j
				int id0 = face.getID(j);
				int id1 = face.getID(mod(j + 1, count));
				int id2 = face.getID(mod(j - 1, count));

				auto pt1 = vertList.at(id0).getPosition();
				auto pt2 = vertList.at(id1).getPosition();
				auto pt3 = vertList.at(id2).getPosition();

				vec3d v1 = pt2 - pt1;
				vec3d v2 = pt3 - pt1;
				vec3d normal = v1.crossProduct(v2).normalize();

				// Ajout de la normale au calcul de la moyenne
				vec3d &inplace = normalSum.at(id0);
				inplace = inplace + normal;

				normalCount.at(id0)++;
			}
		}

		// On set les normales
		for (int i = 0; i < normalSum.size(); i++) {
			int count = normalCount.at(i);
			vec3d normal;

			if (count != 0) {
				normal = normalSum.at(i) / count;
			} else {
				normal = vec3d(0, 0, 1);
			}

			Vertex &vn = mesh.getVertex(i);
			vn.setNormal(normal);
		}
	}
}
