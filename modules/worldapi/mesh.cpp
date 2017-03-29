#include "mesh.h"

#include <iostream>
#include <stdexcept>

//----- VERTEX
Vertex::Vertex(VType type) : _type(type) {}
Vertex::~Vertex() {}

Vertex & Vertex::add(float value) {
	_values.push_back(value);
	return *this;
}

void Vertex::setID(int id) {
	_vID = id;
}


//----- FACE

Face::Face() : _vcount(0) {}
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

int Face::getID(VType type, int vert) const {
	if (vert >= _vcount) throw std::runtime_error("Erreur : sommet inexistant");
	return getIDs(type)[vert];
}

const std::vector<int> & Face::getIDs(VType type) const {
	switch (type) {

	case VType::POSITION:
		return _positionIDs;
	case VType::NORMAL:
		return _normalIDs;
	case VType::TEXTURE:
		return _textureIDs;
	case VType::PARAM:
		return _paramIDs;
	default:
		throw std::runtime_error("Ce type de sommet n'est pas supporté");
	}
}

void Face::setID(VType type, int vert, int id) {
	if (vert >= _vcount) throw std::runtime_error("Erreur : sommet inexistant");
	getIDsInternal(type)[vert] = id;
}

std::vector<int> & Face::getIDsInternal(VType type) {
	switch (type) {

	case VType::POSITION:
		return _positionIDs;
	case VType::NORMAL:
		return _normalIDs;
	case VType::TEXTURE:
		return _textureIDs;
	case VType::PARAM:
		return _paramIDs;
	default:
		throw std::runtime_error("Ce type de sommet n'est pas supporté");
	}
}


//----- MESH

Mesh::Mesh() {}
Mesh::~Mesh() {}

void Mesh::addVertex(Vertex & vertex) {
	std::vector<Vertex> & vertexList = getList(vertex.getType());
	int id = vertexList.size();
	vertexList.push_back(vertex);
	vertexList.at(id).setID(id);
}

void Mesh::addFace(Face & face) {
	_faces.push_back(face);
}

const std::vector<Face> & Mesh::getFaces() const {
	return _faces;
}

const std::vector<Vertex> & Mesh::getVertices(VType type) const {
	switch (type) {

	case VType::POSITION:
		return _positions;
	case VType::NORMAL:
		return _normals;
	case VType::TEXTURE:
		return _textures;
	case VType::PARAM:
		return _params;
	default:
		throw std::runtime_error("Ce type de sommet n'est pas supporté");
	}
}

std::vector<Vertex> & Mesh::getList(VType vtype) {
	switch (vtype) {

	case VType::POSITION:
		return _positions;
	case VType::NORMAL:
		return _normals;
	case VType::TEXTURE:
		return _textures;
	case VType::PARAM:
		return _params;
	default:
		throw std::runtime_error("Ce type de sommet n'est pas supporté");
	}
}

int Mesh::getCount(const VType & type) const {
	return getVertices(type).size();
}

void Mesh::clearVertices(VType type) {
	getList(type).clear();
}

void Mesh::allocate(VType type, int count) {
	getList(type).reserve(count);
}

void Mesh::allocateFaces(int count) {
	_faces.reserve(count);
}

void Mesh::setMaterialName(std::string material) {
	_materialName = material;
}

void Mesh::optimize() {
	

	for (Vertex & vert : getList(VType::POSITION)) {

	}
}
