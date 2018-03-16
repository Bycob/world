#include "Mesh.h"

#include <iostream>
#include <stdexcept>


//----- FACE

Face::Face()  {
	this->_ids.reserve(3);
}
Face::~Face() {}

void Face::addID(int id) {
	_ids.push_back(id);
}

int Face::getID(int vert) const {
	return _ids[vert];
}

void Face::setID(int vert, int id) {
	_ids[vert] = id;
}

const std::vector<int>& Face::getIDs() const {
	return _ids;
}

int Face::vertexCount() const { 
	return (int)_ids.size();
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

const Vertex & Mesh::getVertex(int id) const {
	return _vertices.at(id);
}

Vertex & Mesh::getVertex(int id) {
	return _vertices.at(id);
}

void Mesh::allocateFaces(int count) {
	_faces.reserve(count);
}

const std::vector<Vertex> & Mesh::getVertices() const {
	return _vertices;
}

int Mesh::getCount() const {
	return _vertices.size();
}

void Mesh::clearVertices() {
	_vertices.clear();
}

void Mesh::allocateVertices(int count) {
	_vertices.reserve(count);
}

void Mesh::addVertex(const Vertex & vertex) {
	_vertices.push_back(vertex);
}

void Mesh::setMaterialName(const std::string & name) {
	_materialName = name;
}

const std::string& Mesh::getMaterialName() const {
	return _materialName;
}
