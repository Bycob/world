#include "Mesh.h"

#include <iostream>
#include <stdexcept>


//----- FACE

Face::Face() : _vcount(0) {
	this->_positionIDs.reserve(3);
	this->_normalIDs.reserve(3);
	this->_textureIDs.reserve(3);
	this->_paramIDs.reserve(3);
}
Face::~Face() {}

void Face::addVertex(int vertexID, int normalID, int textureID, int paramID) {
	if (vertexID < 0) {
		std::cerr << "Face::addVertex : indice non valide : " << vertexID << std::endl;
		throw std::runtime_error("");
	}

	//std::cout << "v" << vertexID << " vt" << textureID << " vn" << normalID << std::endl;

	this->_positionIDs.push_back(vertexID);
	this->_normalIDs.push_back(normalID);
	this->_textureIDs.push_back(textureID);
	this->_paramIDs.push_back(paramID);

	_vcount++;
}

void Face::addVertexUniqueID(int vertexID) {
	addVertex(vertexID, vertexID, vertexID, vertexID);
}


//----- MESH

Mesh::Mesh() {}
Mesh::~Mesh() {}

void Mesh::addFace(Face & face) {
	_faces.push_back(face);
}

const std::vector<Face> & Mesh::getFaces() const {
	return _faces;
}

void Mesh::allocateFaces(int count) {
	_faces.reserve(count);
}

void Mesh::setMaterialName(const std::string & name) {
	_materialName = name;
}

void Mesh::optimize() {
	
	for (Vertex<VType::POSITION> & vert : getList<VType::POSITION>()) {

	}
}