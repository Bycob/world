#pragma once

#include "worldapidef.h"

#include <string>
#include <vector>
#include <stdexcept>

#include "maths/MathsHelper.h"

enum class VType {
	POSITION, NORMAL, TEXTURE, PARAM
};

template<VType type>
class Vertex {
public :
	Vertex() {
		_values.reserve(3);
	}

	~Vertex() {}

	Vertex<type> & add(float value) {
		_values.push_back(value);
		return *this;
	}

	const std::vector<float> & getValues() const { return _values; }

	void setID(int id) {
		_vID = id;
	}
	int getID() const { return _vID; };

	maths::vec3d toVec3() const;
	maths::vec2d toVec2() const;
private :
	int _vID;
	std::vector<float> _values;
};

template class Vertex<VType::POSITION>;
template class Vertex<VType::NORMAL>;
template class Vertex<VType::TEXTURE>;
template class Vertex<VType::PARAM>;


class WORLDAPI_EXPORT Face {

public :
	Face();
	virtual ~Face();

	void addVertex(int vertexID, int normalID = -1, int textureID = -1, int paramID = -1);
	void addVertexUniqueID(int vertexID);

	template <VType type> int getID(int vert) const;
	template <VType type> const std::vector<int> & getIDs() const;

	/** Change la composante de type "type" du sommet "vert" en "id" */
	template <VType type> void setID(int vert, int id);

	int vertexCount() const { return _vcount; }
private :
	int _vcount;
	std::vector<int> _positionIDs;
	std::vector<int> _normalIDs;
	std::vector<int> _textureIDs;
	std::vector<int> _paramIDs;

	template<VType type> std::vector<int> & getIDsInternal();
};


class WORLDAPI_EXPORT Mesh {

public :
	Mesh();
	virtual ~Mesh();

	template <VType type> void addVertex(Vertex<type> & vertex);

	/* Ajoute une face au mesh. Cette méthode n'effectue aucune
	vérification quand à l'existence des vertices correspondant
	aux indices de la face. */
	void addFace(Face & face);

	const std::vector<Face> & getFaces() const;

	template <VType type> const std::vector<Vertex<type>> & getVertices() const;

	template <VType type> int getCount() const;

	template <VType type> void clearVertices();

	template <VType type> void allocateVertices(int capacity);

	void allocateFaces(int capacity);

	void setMaterialName(const std::string & material);
	
	const std::string & getMaterialName() const {
		return _materialName;
	}

	/* Optimise le mesh en fusionnant les vertices identiques. */
	void optimize();
private:
	std::string _materialName;

	std::vector<Vertex<VType::POSITION>> _positions;
	std::vector<Vertex<VType::NORMAL>> _normals;
	std::vector<Vertex<VType::TEXTURE>> _textures;
	std::vector<Vertex<VType::PARAM>> _params;
	std::vector<Face> _faces;

	template <VType type> std::vector<Vertex<type>> & getList();
};

#include "Mesh.inl"