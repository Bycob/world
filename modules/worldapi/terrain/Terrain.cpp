#include "Terrain.h"

#include <algorithm>
#include <iostream>
#include <math.h>

#include <worldapi/maths/MathsHelper.h>
#include <worldapi/maths/Interpolation.h>

#include "../Interop.h"
#include "../Image.h"
#include "../Mesh.h"
#include "../MeshOps.h"
#include "../Stream.h"

using namespace maths;
using namespace img;
using namespace arma;

Terrain::Terrain(int size) : 
	_array(size, size),
	_texture(std::make_unique<Image>(1, 1, ImageType::RGB)),
	_bbox({-0.5, -0.5, -0.0}, {0.5, 0.5, 0.4}) {

}

Terrain::Terrain(const Mat<double> & data) :
	_array(data), 
	_texture(nullptr),
	_bbox({-0.5, -0.5, -0.0}, {0.5, 0.5, 0.4}) {

	if (data.n_rows != data.n_cols) {
		throw std::runtime_error("Terrain must be squared !");
	}
}

Terrain::Terrain(const Terrain &terrain)
		: _array(terrain._array),
		  _bbox(terrain._bbox) {

	if (terrain._texture != nullptr) {
		_texture = std::make_unique<Image>(*terrain._texture);
	}
}

Terrain::Terrain(Terrain &&terrain)
		: _array(std::move(terrain._array)),
		  _texture(std::move(terrain._texture)),
		  _bbox(terrain._bbox){

}

Terrain::~Terrain() = default;

Terrain& Terrain::operator=(const Terrain &terrain) {
	_array = terrain._array;
	_bbox = terrain._bbox;

	if (terrain._texture != nullptr) {
		_texture = std::make_unique<Image>(*terrain._texture);
	}

	return *this;
}

void Terrain::setBounds(double xmin, double ymin, double zmin, double xmax, double ymax, double zmax) {
	_bbox.reset({xmin, ymin, zmin}, {xmax, ymax, zmax});
}

const maths::BoundingBox& Terrain::getBoundingBox() const {
	return _bbox;
}

double& Terrain::operator()(int x, int y) {
	return _array(x, y);
}

double Terrain::getZ(double x, double y) const {
	// TODO gerer depassement de bornes
	int posX = (int) (x * (_array.n_rows - 1));
	int posY = (int) (y * (_array.n_cols - 1));

	return _array(posX, posY);
}

double Terrain::getZInterpolated(double x, double y) const {
	int width = (int) (_array.n_rows - 1);
	int height = (int) (_array.n_cols - 1);

	x *= width;
	y *= height;
	int posX1 = clamp((int) floor(x), 0, width);
	int posY1 = clamp((int) floor(y), 0, height);
	int posX2 = min(posX1 + 1, width);
	int posY2 = min(posY1 + 1, height);

	double ip1 = interpolateLinear(posX1, _array(posX1, posY1), posX2, _array(posX2, posY1), x);
	double ip2 = interpolateLinear(posX1, _array(posX1, posY2), posX2, _array(posX2, posY2), x);

	return interpolateLinear(posY1, ip1, posY2, ip2, y);
}

Mesh * Terrain::convertToMesh() {
	auto lower = _bbox.getLowerBound();
	auto size = _bbox.getUpperBound() - lower;
	return convertToMesh(lower.x, lower.y, lower.z, size.x, size.y, size.z);
}

Mesh * Terrain::convertToMesh(double sizeX, double sizeY, double sizeZ) const {
	return convertToMesh(-sizeX / 2, -sizeY / 2, 0, sizeX, sizeY, sizeZ);
}

Mesh * Terrain::convertToMesh(double offsetX, double offsetY, double offsetZ, double sizeX, double sizeY, double sizeZ) const {
	Mesh * mesh = new Mesh();

	// Réservation de mémoire
	int vertCount = _array.n_rows * _array.n_cols;
	mesh->allocateVertices(vertCount);

	//Vertices
	int i = 0;
	for (int x = 0; x < _array.n_rows; x++) {
		for (int y = 0; y < _array.n_cols; y++) {
			float xd = (float)x / (_array.n_rows - 1);
			float yd = (float)y / (_array.n_cols - 1);

			Vertex vert;

			vert.setPosition(xd * sizeX + offsetX, yd * sizeY + offsetY, _array(x, y) * sizeZ + offsetZ);
			vert.setTexture(xd, 1 - yd);

			mesh->addVertex(vert);
		}
	}

	//Faces
	auto indice = [this](int x, int y)->int { return x * this->_array.n_cols + y; };
	mesh->allocateFaces((_array.n_rows - 1) * (_array.n_cols - 1) * 2);

	for (int x = 0; x < _array.n_rows - 1; x++) {
		for (int y = 0; y < _array.n_cols - 1; y++) {
			Face face1, face2;
			
			face1.addID(indice(x, y));
			face1.addID(indice(x + 1, y));
			face1.addID(indice(x, y + 1));

			face2.addID(indice(x + 1, y + 1));
			face2.addID(indice(x, y + 1));
			face2.addID(indice(x + 1, y));

			mesh->addFace(face1);
			mesh->addFace(face2);
		}
	}

	MeshOps::recalculateNormals(*mesh);

	return mesh;
}

Image Terrain::convertToImage() const {
	return Image(this->_array);
}

char * Terrain::getRawData(int &rawDataSize, float height, float offset) const {
	float * result = new float[_array.n_rows * _array.n_cols];

	for (int x = 0 ; x < _array.n_rows ; x++) {
		for (int y = 0 ; y < _array.n_cols ; y++) {
			result[x * _array.n_cols + y] = (float) _array(x, y) * height + offset;
		}
	}

	rawDataSize = getRawDataSize();

	return (char*) result;
}

int Terrain::getRawDataSize() const {
	return (int) (_array.n_rows * _array.n_cols) * sizeof(float) / sizeof(char);
}

Image Terrain::getTexture() const {
	if (_texture == nullptr) throw std::runtime_error("No texture");
	return *_texture;
}

const Image & Terrain::texture() const {
	if (_texture == nullptr) throw std::runtime_error("No texture");
	return *_texture;
}

vec2i Terrain::getPixelPos(double x, double y) const {
	return {
		(int) maths::min(x * _array.n_rows, _array.n_rows - 1),
		(int) maths::min(y * _array.n_cols, _array.n_cols - 1)
	};
}
