#pragma once

#include <string>
#include <vector>

#include "worldapidef.h"

enum class VType {
	POSITION, NORMAL, TEXTURE, PARAM
};


class WORLDAPI_EXPORT Vertex {
public :
	Vertex(VType type);
	virtual ~Vertex();

	Vertex & add(float value);
	const std::vector<float> & getValues() const { return _values; }

	void setID(int id);
	int getID() const { return _vID; };

	void setType(VType type) { _type = type; }
	VType getType() const { return _type; }

private :
	int _vID;
	VType _type;
	std::vector<float> _values;
};


class WORLDAPI_EXPORT Face {

public :
	Face();
	virtual ~Face();

	void addVertex(int vertexID, int normalID = -1, int textureID = -1, int paramID = -1);
	void addVertexUniqueID(int vertexID);

	int getID(VType type, int vert) const;
	const std::vector<int> & getIDs(VType type) const;

	/** Change la composante de type "type" du sommet "vert" en "id" */
	void setID(VType type, int vert, int id);

	int vertexCount() const { return _vcount; }
private :
	int _vcount;
	std::vector<int> _positionIDs;
	std::vector<int> _normalIDs;
	std::vector<int> _textureIDs;
	std::vector<int> _paramIDs;

	std::vector<int> & getIDsInternal(VType type);
};


class WORLDAPI_EXPORT Mesh {

public :
	Mesh();
	virtual ~Mesh();

	void addVertex(Vertex & vertex);

	/* Ajoute une face au mesh. Cette méthode n'effectue aucune
	vérification quand à l'existence des vertices correspondant
	aux indices de la face. */
	void addFace(Face & face);

	const std::vector<Face> & getFaces() const;
	const std::vector<Vertex> & getVertices(VType type) const;
	int getCount(const VType & type) const;

	void clearVertices(VType type);

	void setMaterialName(std::string material);
	std::string getMaterialName() const {
		return _materialName;
	}

	/* Optimise le mesh en fusionnant les vertices identiques. */
	void optimize();
private:
	std::string _materialName;

	std::vector<Vertex> _positions;
	std::vector<Vertex> _normals;
	std::vector<Vertex> _textures;
	std::vector<Vertex> _params;
	std::vector<Face> _faces;

	std::vector<Vertex> & getList(VType type);
};
