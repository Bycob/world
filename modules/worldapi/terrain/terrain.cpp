#include <algorithm>
#include <iostream>
#include <math.h>

#include <worldapi/maths/mathshelper.h>
#include <worldapi/maths/interpolation.h>

#include "terrain.h"
#include "../interop.h"
#include "../Image.h"
#include "../mesh.h"
#include "../meshop.h"
#include "../stream.h"

using namespace maths;
using namespace img;
using namespace arma;

Terrain::Terrain(int size) : 
	_array(size, size),
	_texture(std::make_unique<Image>(1, 1, ImageType::RGB)) {

}

Terrain::Terrain(const Mat<double> & data) :
	_array(data), 
	_texture(nullptr) {

	if (data.n_rows != data.n_cols) {
		throw std::runtime_error("Terrain must be squared !");
	}
}

Terrain::Terrain(const Terrain &terrain)
		: _array(terrain._array),
		  _texture(std::make_unique<Image>(*terrain._texture)),
		  _subdivisionStage(terrain._subdivisionStage),
		  _noisePart(terrain._noisePart),
		  _x(terrain._x),
		  _y(terrain._y),
		  _parent(nullptr),
		  _subdivideFactor(terrain._subdivideFactor) {

	for (auto & ptr : terrain._subterrain) {
		_subterrain.emplace_back(new Terrain(*ptr));
	}
	for (auto & ptr : _subterrain) {
		ptr->_parent = this;
	}
}

Terrain::Terrain(Terrain &&terrain)
		: _array(std::move(terrain._array)),
		  _texture(std::move(terrain._texture)),
		  _subdivisionStage(terrain._subdivisionStage),
		  _noisePart(terrain._noisePart),
		  _x(terrain._x),
		  _y(terrain._y),
		  _parent(nullptr),
		  _subdivideFactor(terrain._subdivideFactor) {

	for (auto & ptr : terrain._subterrain) {
		_subterrain.push_back(std::move(ptr));
	}
	for (auto & ptr : _subterrain) {
		ptr->_parent = this;
	}
}

Terrain::~Terrain() {

}

void Terrain::getLocalPosition(const Terrain & child, double globalPosX, double globalPosY, double & localPosX, double & localPosY) const {
	double reductionFactor = 1;
	double offsetX = 0;
	double offsetY = 0;
	const Terrain* interTerrain = &child;

	if (interTerrain == this) {
		localPosX = globalPosX;
		localPosY = globalPosY;
	}
	else {
		while (interTerrain != this) {
			reductionFactor /= interTerrain->_parent->_subdivideFactor;
			offsetX = ((double)interTerrain->_x + offsetX) / interTerrain->_parent->_subdivideFactor;
			offsetY = ((double)interTerrain->_y + offsetY) / interTerrain->_parent->_subdivideFactor;

			interTerrain = interTerrain->_parent;
		}

		localPosX = (globalPosX - offsetX) / reductionFactor;
		localPosY = (globalPosY - offsetY) / reductionFactor;
	}
}

double Terrain::getZ(double x, double y, int stage) const {
	const Terrain & terrain = getSubterrainAt(x, y, stage);

	double posXd, posYd;
	getLocalPosition(terrain, x, y, posXd, posYd);

	int posX = (int) (posXd * terrain._array.n_rows);
	int posY = (int) (posYd * terrain._array.n_cols);

	return terrain._array(posX, posY);
}

double Terrain::getZInterpolated(double x, double y, int stage) const {
	const Terrain & terrain = getSubterrainAt(x, y, stage);

	int width = (int)terrain._array.n_rows;
	int height = (int)terrain._array.n_cols;

	// Positions flottantes sur le subterrain selectionné
	double posXd, posYd;
	getLocalPosition(terrain, x, y, posXd, posYd);
	
	posXd *= width;
	posYd *= height;
	// TODO définir le comportement de manière plus exacte
	int posX1 = min(width - 1, (int) floor(posXd));
	int posY1 = min(height - 1, (int) floor(posYd));
	int posX2 = posX1 + 1; if (posX2 >= width) posX2 = posX1;
	int posY2 = posY1 + 1; if (posY2 >= height) posY2 = posY1;

	double ip1 = interpolateLinear(posX1, terrain._array(posX1, posY1), posX2, terrain._array(posX2, posY1), posXd);
	double ip2 = interpolateLinear(posX1, terrain._array(posX1, posY2), posX2, terrain._array(posX2, posY2), posXd);

	return interpolateLinear(posY1, ip1, posY2, ip2, posYd);
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

const Terrain & Terrain::getSubterrainAt(double x, double y, int stage) const {
	if (stage == 0 || !this->isSubdivided()) {
		const Terrain & result = *this;
		return result;
	}

	int xindex = (int) floor(x * _subdivideFactor);
	int yindex = (int) floor(y * _subdivideFactor);

	Terrain & subterrain = getSubterrain(xindex, yindex);
	double subx = x * _subdivideFactor - xindex;
	double suby = y * _subdivideFactor - yindex;

	return subterrain.getSubterrainAt(subx, suby, stage - 1);
}

Terrain & Terrain::getSubterrain(int xindex, int yindex) const {
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

	// Réservation de mémoire
	int vertCount = _array.n_rows * _array.n_cols;
	mesh->allocateVertices<VType::POSITION>(vertCount);
	mesh->allocateVertices<VType::TEXTURE>(vertCount);
	mesh->allocateVertices<VType::NORMAL>(vertCount);

	//Vertices
	int i = 0;
	for (int x = 0; x < _array.n_rows; x++) {
		for (int y = 0; y < _array.n_cols; y++) {
			float xd = (float)x / (_array.n_rows - 1);
			float yd = (float)y / (_array.n_cols - 1);

			Vertex<VType::POSITION> vert;

			vert.add(xd * sizeX + offsetX)
				.add(yd * sizeY + offsetY)
				.add(_array(x, y) * sizeZ + offsetZ);

			mesh->addVertex(vert);


			Vertex<VType::TEXTURE> vertext;

			vertext.add(xd)
				.add(1 - yd);

			mesh->addVertex(vertext);
		}
	}

	//Faces
	auto indice = [this](int x, int y)->int { return x * this->_array.n_cols + y; };
	mesh->allocateFaces((_array.n_rows - 1) * (_array.n_cols - 1) * 2);

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

Image Terrain::convertToImage() const {
	return Image(this->_array);
}

void Terrain::writeRawData(std::ostream &stream) const {
	bin_ostream binstream(stream);

	for (int x = 0 ; x < _array.n_rows ; x++) {
		for (int y = 0 ; y < _array.n_cols ; y++) {
			binstream << (float) _array(x, y);
		}
	}
}

int Terrain::getRawDataSize() const {
	return (int) (_array.n_rows * _array.n_cols) * sizeof(float);
}

Image Terrain::getTexture() const {
	if (_texture == nullptr) throw std::runtime_error("No texture");
	return *_texture;
}

const Image & Terrain::texture() const {
	if (_texture == nullptr) throw std::runtime_error("No texture");
	return *_texture;
}
