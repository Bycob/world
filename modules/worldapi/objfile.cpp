#define TINYOBJLOADER_IMPLEMENTATION
#include <iostream>
#include <fstream>
#include <string>

#include "mesh.h"
#include "Material.h"
#include "stringops.h"

#include "objfile.h"

ObjLoader::ObjLoader() {
	_defaultMaterial = std::make_shared<Material>(DEFAULT_MATERIAL_NAME);
}

ObjLoader::ObjLoader(std::string pathname, bool triangulate) : ObjLoader() {
	_filename = pathname;
	read(triangulate);
}

ObjLoader::~ObjLoader() {

}

void ObjLoader::printDebug() {
	std::cout << "Ce fichier objet contient :" << std::endl;

	//TODO afficher la description du fichier
}

void ObjLoader::read(bool triangulate) {
	//Lecture via tinyobj
	std::string errstr = "";
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	
	bool success = tinyobj::LoadObj(shapes, materials, errstr, _filename.c_str(), NULL, triangulate);

	if (!success) {
		throw std::ios_base::failure(errstr);
	}

	//Conversion en meshes
	for (auto shape : shapes) {
		_meshes.push_back(std::make_shared<Mesh>());

		Mesh & mesh = **_meshes.end();

		//positions
		std::vector<float> & positions = shape.mesh.positions;
		assert(positions.size() % 3 == 0);

		for (int i = 0; i < positions.size() / 3; i++) {
			Vertex vert(VType::POSITION);

			vert.add(positions.at(i * 3))
				.add(positions.at(i * 3 + 1))
				.add(positions.at(i * 3 + 2));

			mesh.addVertex(vert);
		}

		//normales
		std::vector<float> & normals = shape.mesh.normals;
		assert(normals.size() % 3 == 0);

		for (int i = 0; i < normals.size() / 3; i++) {
			Vertex vert(VType::NORMAL);

			vert.add(normals.at(i * 3))
				.add(normals.at(i * 3 + 1))
				.add(normals.at(i * 3 + 2));

			mesh.addVertex(vert);
		}

		//texture
		std::vector<float> & textures = shape.mesh.texcoords;
		assert(textures.size() % 2 == 0);

		for (int i = 0; i < textures.size() / 2; i++) {
			Vertex vert(VType::TEXTURE);

			vert.add(textures.at(i * 2))
				.add(textures.at(i * 2 + 1));

			mesh.addVertex(vert);
		}

		//faces
		int offset = 0;
		for (int ngon : shape.mesh.num_vertices) {
			Face face;

			for (int i = offset; i < offset + ngon; i++) {
				int indice = shape.mesh.indices.at(i);

				//Dans tinyObjLoader, l'indice est le même pour tout, et les vertices sont dupliquées.
				int vt = mesh.getVertices(VType::TEXTURE).size() > indice ? indice : -1;
				int vn = mesh.getVertices(VType::NORMAL).size() > indice ? indice : -1;

				face.addVertex(indice, vn, vt);
			}

			offset += ngon;
			mesh.addFace(face);
		}
	}
}

void ObjLoader::write(std::string filename) {
	//On enlève l'extension si elle est passée en paramètres.
	if (endsWith(filename, ".obj")) {
		filename = filename.substr(0, filename.size() - 4);
	}

	//Obtention du stream pour le fichier .obj
	std::ofstream objfile(filename + ".obj");
	if (!objfile.is_open()) {
		throw std::ios_base::failure("oh, shit, we didnt wrote u're file, " + filename + ".obj... sorry for the disagreement :-*");
	}

	//Obtention du stream pour le fichier .mtl
	std::ofstream mtlfile(filename + ".mtl");
	if (!mtlfile.is_open()) {
		throw std::ios_base::failure("oh, shit, we didnt wrote u're file, " + filename + ".mtl... sorry for the disagreement :-*");
	}

	int lastSlashIndex = filename.find_last_of("/\\");
	if (lastSlashIndex == std::string::npos) {
		lastSlashIndex = 0;
	}
	std::string localName = filename.substr(lastSlashIndex + 1);
	objfile << "mtllib " << localName << ".mtl" << std::endl;

	//Ecriture
	write(objfile, mtlfile);

	objfile.close();
	mtlfile.close();
}

void ObjLoader::write(std::ostream & objstream, std::ostream & mtlstream) {
	int meshCount = 1;
	int verticesRead = 0;
	int normalRead = 0;
	int texCoordRead = 0;

	std::string defaultMaterialName = _materials.size() == 0 ? _defaultMaterial->getName() : _materials[0]->getName();
	bool includeDefaultMaterial = _materials.size() == 0;

	for (std::shared_ptr<Mesh> & mesh : _meshes) {
		std::string name = std::string("mesh") + std::to_string(meshCount);
		objstream << "o " << name << std::endl;

		//Materiau
		objstream << "usemtl ";
		std::string materialName = mesh->getMaterialName();

		if (materialName == "") {
			objstream << defaultMaterialName;
		}
		else if (materialName == _defaultMaterial->getName()) {
			includeDefaultMaterial = true;
		}
		else {
			objstream << materialName;
		}
		objstream << std::endl;

		int verticesOffset = verticesRead;
		int normalOffset = normalRead;
		int texCoordOffset = texCoordRead;

		// Lambda pour écrire les valeurs
		auto writeValues = [&file = objstream](const Vertex & vert) {
			const std::vector<float> & values = vert.getValues();
			for (const float & value : values) {
				file << " " << value;
			}

			file << std::endl;
		};

		// Ecriture des vertices
		const std::vector<Vertex> & vertList = mesh->getVertices(VType::POSITION);

		for (const Vertex & vert : vertList) {
			verticesRead++;
			objstream << "v";
			writeValues(vert);
		}

		// Ecriture des normales
		const std::vector<Vertex> & normalList = mesh->getVertices(VType::NORMAL);

		for (const Vertex & normal : normalList) {
			normalRead++;
			objstream << "vn";
			writeValues(normal);
		}

		// Ecriture des coordonnées de texture
		const std::vector<Vertex> & texCoordList = mesh->getVertices(VType::TEXTURE);

		for (const Vertex & texCoord : texCoordList) {
			texCoordRead++;
			objstream << "vt";
			writeValues(texCoord);
		}

		// Ecriture des faces
		const std::vector<Face> & faceList = mesh->getFaces();

		for (const Face & face : faceList) {
			objstream << "f";

			const std::vector<int> vertIndices = face.getIDs(VType::POSITION),
				texCoordIndices = face.getIDs(VType::TEXTURE),
				normalIndices = face.getIDs(VType::NORMAL);

			for (int i = 0; i < vertIndices.size(); i++) {
				objstream << " " << vertIndices[i] + verticesOffset + 1 << "/";
				if (i < texCoordIndices.size() && texCoordIndices[i] != -1 && texCoordIndices[i] < texCoordRead) {
					objstream << texCoordIndices[i] + texCoordOffset + 1;
				}
				objstream << "/";
				if (i < normalIndices.size() && normalIndices[i] != -1 && normalIndices[i] < normalRead) {
					objstream << normalIndices[i] + normalOffset + 1;
				}
			}
			objstream << std::endl;
		}

		meshCount++;
	}


	// Ajout des matériaux
	// On vérifie que le nom du "default material" n'est pas déjà pris
	if (includeDefaultMaterial) {
		for (std::shared_ptr<Material> & material : _materials) {
			if (material->getName() == _defaultMaterial->getName()) {
				includeDefaultMaterial = false;
				break;
			}
		}
	}

	if (includeDefaultMaterial) {
		_materials.push_back(_defaultMaterial);
	}

	int materialID = 1;

	for (std::shared_ptr<Material> material : _materials) {
		//Déclaration du materiau
		mtlstream << std::endl << "newmtl ";
		std::string materialName = trimSpaces(material->getName());
		
		if (materialName == "") {
			materialName = "material" + std::to_string(materialID);
			materialID++;
		}
		mtlstream << materialName << std::endl;

		//Kd, Ka, Ks
		auto writeData = [&mtlstream=mtlstream](const std::string & name, const arma::vec value)  {
			mtlstream << name;
			for (auto val : value) {
				mtlstream << " " << val;
			}
			mtlstream << std::endl;
		};
		writeData("Kd", material->getKd());
		writeData("Ka", material->getKa());
		writeData("Ks", material->getKs());

		//maps
		auto writeMap = [&mtlstream = mtlstream](const std::string & name, const std::string & value) {
			if (value != "") {
				mtlstream << name << " " << value << std::endl;
			}
		};
		writeMap("map_Kd", material->getMapKd());
	}

	if (includeDefaultMaterial) {
		_materials.pop_back();
	}
}

///PRIVATE : méthode permettant d'agrandir un vector avant d'accéder à l'élément
///@returns Une référence vers l'élement à l'emplacement <tt>index</tt>
template <typename T>
T & extAt(std::vector<T> & vector, int index) {
	while (vector.size() <= index) {
		vector.push_back(0);
	}

	return vector.at(index);
}


void ObjLoader::addMesh(std::shared_ptr<Mesh> mesh) {
	_meshes.push_back(mesh);
}

void ObjLoader::addMesh(const Mesh & mesh) {
	_meshes.push_back(std::make_shared<Mesh>(mesh));
}

void ObjLoader::addMaterial(std::shared_ptr<Material> material) {
	_materials.push_back(material);
}

void ObjLoader::getMeshes(std::vector<std::shared_ptr<Mesh>> &output) {
	for (auto mesh : _meshes) {
		output.push_back(mesh);
	}
}

void ObjLoader::getMaterials(std::vector<std::shared_ptr<Material>> &output) {
	for (auto mesh : _materials) {
		output.push_back(mesh);
	}
}
