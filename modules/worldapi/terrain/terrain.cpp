#include <algorithm>
#include <iostream>
#include <math.h>

#include <worldapi/maths/mathshelper.h>

#include "terrain.h"
#include "../interop.h"
#include "../Image.h"
#include "../mesh.h"
#include "../meshop.h"

using namespace maths;
using namespace img;

Terrain::Terrain(int size) : _array(size, size) {

}

Terrain::~Terrain() {

}

void Terrain::getLocalPosition(Terrain & child, double globalPosX, double globalPosY, double * const localPosX, double * const localPosY) {
	double reductionFactor = 1;
	double offsetX = 0;
	double offsetY = 0;
	Terrain* interTerrain = &child;

	if (interTerrain == this) {
		*localPosX = globalPosX;
		*localPosY = globalPosY;
	}
	else {
		while (interTerrain != this) {
			reductionFactor /= interTerrain->_parent->_subdivideFactor;
			offsetX = ((double)interTerrain->_x + offsetX) / interTerrain->_parent->_subdivideFactor;
			offsetY = ((double)interTerrain->_y + offsetY) / interTerrain->_parent->_subdivideFactor;

			interTerrain = interTerrain->_parent;
		}

		*localPosX = (globalPosX - offsetX) / reductionFactor;
		*localPosY = (globalPosY - offsetY) / reductionFactor;
	}
}

double Terrain::getZ(double x, double y, int stage) {
	Terrain & terrain = getSubterrainAt(x, y, stage);

	double posXd, posYd;
	getLocalPosition(terrain, x, y, &posXd, &posYd);

	int posX = (int)floor(posXd * terrain._array.n_rows);
	int posY = (int)floor(posYd * terrain._array.n_cols);

	return terrain._array(x, y);
}

double Terrain::getZInterpolated(double x, double y, int stage) {
	Terrain & terrain = getSubterrainAt(x, y, stage);

	double posXd, posYd;
	getLocalPosition(terrain, x, y, &posXd, &posYd);
	
	double posX = posXd * terrain._array.n_rows;
	double posY = posYd * terrain._array.n_cols;
	int posX1 = (int) floor(posX);
	int posY1 = (int) floor(posY);
	int posX2 = posX1 + 1; if (posX2 >= terrain._array.n_rows) posX2 = posX1;
	int posY2 = posY1 + 1; if (posY2 >= terrain._array.n_cols) posY2 = posY1;

	double ip1 = interpolateLinear(posX1, terrain._array(posX1, posY1), posX2, terrain._array(posX2, posY1), posX);
	double ip2 = interpolateLinear(posX1, terrain._array(posX1, posY2), posX2, terrain._array(posX2, posY2), posX);

	return interpolateLinear(posY1, ip1, posY2, ip2, posY);
}

bool Terrain::isSubdivided() const {
	return _subdivideFactor != 0;
}

int Terrain::getLevelDiff(const Terrain & terrain) const {
	return _subdivisionStage - terrain._subdivisionStage;
}

void Terrain::subdivide(int subdivideFactor) {
	_subdivideFactor = subdivideFactor;
	_subterrain = std::vector<std::unique_ptr<Terrain>>(subdivideFactor * subdivideFactor);

	for (int x = 0; x < subdivideFactor; x++) {
		for (int y = 0; y < subdivideFactor; y++) {
			std::unique_ptr<Terrain> terrain = std::make_unique<Terrain>(_array.n_rows);

			terrain->_parent = this;
			terrain->_subdivisionStage = _subdivisionStage + 1;
			terrain->_x = x;
			terrain->_y = y;

			_subterrain[x * subdivideFactor + y] = std::move(terrain);
		}
	}
}

Terrain & Terrain::getSubterrainAt(double x, double y, int stage) {
	if (stage == 0 || !this->isSubdivided()) {
		return *this;
	}

	int xindex = (int) floor(x * _subdivideFactor);
	int yindex = (int) floor(y * _subdivideFactor);

	std::cout << xindex << " " << yindex << " " << _subdivideFactor << std::endl;
	Terrain & subterrain = getSubterrain(xindex, yindex);
	double subx = x * _subdivideFactor - xindex;
	double suby = y * _subdivideFactor - yindex;

	return subterrain.getSubterrainAt(subx, suby, stage - 1);
}

Terrain & Terrain::getSubterrain(double xindex, double yindex) {
	if (xindex >= _subdivideFactor || yindex >= _subdivideFactor || xindex < 0 || yindex < 0) {
		throw std::runtime_error("given coordinates out of bounds");
	}

	return *_subterrain.at(xindex * _subdivideFactor + yindex);
}

Mesh * Terrain::convertToMesh(float sizeX, float sizeY, float sizeZ) const {
	return convertToMesh(-sizeX / 2, -sizeY / 2, 0, sizeX, sizeY, sizeZ);
}

Mesh * Terrain::convertToMesh(float offsetX, float offsetY, float offsetZ, float sizeX, float sizeY, float sizeZ) const {
	Mesh * mesh = new Mesh();

	//Vertices
	int i = 0;
	for (int x = 0; x < _array.n_rows; x++) {
		for (int y = 0; y < _array.n_cols; y++) {

			Vertex vert(VType::POSITION);

			vert.add((float) x / (_array.n_rows - 1) * sizeX + offsetX)
				.add((float) y / (_array.n_cols - 1) * sizeY + offsetY)
				.add(_array(x, y) * sizeZ + offsetZ);

			mesh->addVertex(vert);


			Vertex vertext(VType::TEXTURE);

			vertext.add((float) y / _array.n_cols)
				.add(1 - (float) x / _array.n_rows);

			mesh->addVertex(vertext);
		}
	}

	//Faces
	auto indice = [this](int x, int y)->int { return x * this->_array.n_cols + y; };

	for (int x = 0; x < _array.n_rows - 1; x++) {
		for (int y = 0; y < _array.n_cols - 1; y++) {
			Face face1, face2;
			
			face1.addVertexUniqueID(indice(x, y));
			face1.addVertexUniqueID(indice(x + 1, y));
			face1.addVertexUniqueID(indice(x, y + 1));

			face2.addVertexUniqueID(indice(x + 1, y + 1));
			face2.addVertexUniqueID(indice(x, y + 1));
			face2.addVertexUniqueID(indice(x + 1, y));

			mesh->addFace(face1);
			mesh->addFace(face2);
		}
	}

	MeshOps::recalculateNormals(*mesh);

	return mesh;
}

Mesh * Terrain::convertToSubmesh(float rootSizeX, float rootSizeY, float rootSizeZ) const {
	float sizeX = rootSizeX;
	float sizeY = rootSizeY;
	float sizeZ = rootSizeZ;
	float offsetX = 0;
	float offsetY = 0;
	float offsetZ = 0;
	
	const Terrain * child = this;

	// Calcul de la taille de ce terrain à partir de la taille du parent, ainsi que de l'offset à appliquer.
	while (child->_parent != nullptr) {
		sizeX /= child->_parent->_subdivideFactor;
		sizeY /= child->_parent->_subdivideFactor;

		offsetX = (offsetX + child->_x) / child->_parent->_subdivideFactor;
		offsetY = (offsetY + child->_y) / child->_parent->_subdivideFactor;

		child = child->_parent;
	}

	offsetX *= rootSizeX;
	offsetY *= rootSizeY;

	return convertToMesh(offsetX - rootSizeX / 2, offsetY - rootSizeY / 2, offsetZ, sizeX, sizeY, sizeZ);
}

Image Terrain::convertToImage() {
	return Image(this->_array);
}

Image Terrain::getTexture() {
	return *_texture;
}

const Image & Terrain::texture() {
	return (*_texture);
}
