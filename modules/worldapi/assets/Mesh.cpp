#include "Mesh.h"

#include <iostream>
#include <stdexcept>

namespace world {

//----- FACE

	Face::Face() : _ids{ 0, 0, 0 } {
		
	}

	Face::~Face() {}

	int Face::getID(int vert) const {
		return _ids[vert];
	}

	void Face::setID(int vert, int id) {
		_ids[vert] = id;
	}

	int Face::vertexCount() const {
		return 3;
	}



//----- MESH

	Mesh::Mesh() {}

	Mesh::~Mesh() {}

	void Mesh::allocateFaces(int count) {
		_faces.reserve(count);
	}

	Face &Mesh::getFace(int id) {
		return _faces.at(id);
	}

	const Face &Mesh::getFace(int id) const {
		return _faces.at(id);
	}

	const std::vector<Face> &Mesh::getFaces() const {
		return _faces;
	}

	void Mesh::addFace(const Face &face) {
		_faces.emplace_back(face);
	}

	Face &Mesh::newFace() {
		_faces.emplace_back();
		return _faces.back();
	}

	void Mesh::allocateVertices(int count) {
		_vertices.reserve(count);
	}

	const Vertex &Mesh::getVertex(int id) const {
		return _vertices.at(id);
	}

	Vertex &Mesh::getVertex(int id) {
		return _vertices.at(id);
	}

	const std::vector<Vertex> &Mesh::getVertices() const {
		return _vertices;
	}
	
	void Mesh::addVertex(const Vertex &vert) {
		_vertices.emplace_back(vert);
	}

	Vertex &Mesh::newVertex() {
		_vertices.emplace_back();
		return _vertices.back();
	}

	int Mesh::getCount() const {
		return _vertices.size();
	}

	// TODO allocateNVertices -> les vertices devraient être allouées mais inutilisées ?
}
