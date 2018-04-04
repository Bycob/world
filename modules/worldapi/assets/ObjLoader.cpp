#define TINYOBJLOADER_IMPLEMENTATION
#include <iostream>
#include <fstream>
#include <string>

#include <tiny_obj_loader/tiny_obj_loader.h>

#include "ObjLoader.h"

#include "Mesh.h"
#include "Scene.h"
#include "core/StringOps.h"

namespace world {

	// TODO delete triangulate parameter (unused)
	ObjLoader::ObjLoader(bool triangulate) : _triangulate(triangulate) {
		_defaultMaterial = std::make_shared<Material>(DEFAULT_MATERIAL_NAME);
	}

	ObjLoader::~ObjLoader() {

	}

	Scene *ObjLoader::read(const std::string &filename) const {
		//Lecture via tinyobj
		std::string errstr = "";
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;

		bool success = tinyobj::LoadObj(shapes, materials, errstr, filename.c_str(), NULL, true);

		if (!success) {
			throw std::ios_base::failure(errstr);
		}

		//Conversion en objet 3D
		Scene *result = new Scene();

		for (auto shape : shapes) {
			std::shared_ptr<Mesh> shared_mesh(std::make_shared<Mesh>());
			Mesh &mesh = *shared_mesh;

			//positions
			std::vector<float> &positions = shape.mesh.positions;
			assert(positions.size() % 3 == 0);

			for (int i = 0; i < positions.size() / 3; i++) {
				Vertex vert;
				vert.setPosition(positions.at(i * 3), positions.at(i * 3 + 1), positions.at(i * 3 + 2));
				mesh.addVertex(vert);
			}

			//normales
			std::vector<float> &normals = shape.mesh.normals;
			assert(normals.size() % 3 == 0);

			for (int i = 0; i < normals.size() / 3; i++) {
				mesh.getVertex(i).setNormal(normals.at(i * 3), normals.at(i * 3 + 1), normals.at(i * 3 + 2));
			}

			//texture
			std::vector<float> &textures = shape.mesh.texcoords;
			assert(textures.size() % 2 == 0);

			for (int i = 0; i < textures.size() / 2; i++) {
				mesh.getVertex(i).setTexture(textures.at(i * 2), textures.at(i * 2 + 1));
			}

			//faces
			int offset = 0;
			for (int ngon : shape.mesh.num_vertices) {
				Face face;

				for (int i = offset; i < offset + ngon; i++) {
					int indice = shape.mesh.indices.at(i);

					face.setID(i - offset, indice);
				}

				offset += ngon;
				mesh.addFace(face);
			}

			result->createObject(shared_mesh);
		}

		return result;
	}

	void ObjLoader::write(const Scene &object3D, std::string filename) const {
		//On enlève l'extension si elle est passée en paramètres.
		if (endsWith(filename, ".obj")) {
			filename = filename.substr(0, filename.size() - 4);
		}

		//Obtention du stream pour le fichier .obj
		std::ofstream objfile(filename + ".obj");
		if (!objfile.is_open()) {
			throw std::ios_base::failure(
					"oh, shit, we didnt wrote u're file, " + filename + ".obj... sorry for the disagreement :-*");
		}

		//Obtention du stream pour le fichier .mtl
		std::ofstream mtlfile(filename + ".mtl");
		if (!mtlfile.is_open()) {
			throw std::ios_base::failure(
					"oh, shit, we didnt wrote u're file, " + filename + ".mtl... sorry for the disagreement :-*");
		}

		//Linking des deux fichiers
		int lastSlashIndex = filename.find_last_of("/\\");
		if (lastSlashIndex == std::string::npos) {
			lastSlashIndex = 0;
		}
		std::string localName = filename.substr(lastSlashIndex + 1);
		objfile << "mtllib " << localName << ".mtl" << std::endl;

		//Création de deux string stream subsituts (pour aller plus vite)

		//Ecriture
		write(object3D, objfile, mtlfile);

		objfile.close();
		mtlfile.close();
	}

// Fonctions utiles
	void writeValues(std::ostream &file, const vec3d &vert) {
		file << " " << vert.x << " " << vert.y << " " << vert.z << std::endl;
	}

	void writeValues(std::ostream &file, const vec2d &vert) {
		file << " " << vert.x << " " << vert.y << std::endl;
	}

	void ObjLoader::write(const Scene &scene, std::ostream &objstream, std::ostream &mtlstream) const {
		std::vector<std::shared_ptr<Material>> materials;
		scene.getMaterials(materials);
		std::vector<Object3D *> objects;
		scene.getObjects(objects);

		int meshCount = 1;
		int verticesRead = 0;
		int normalRead = 0;
		int texCoordRead = 0;

		std::string defaultMaterialName = materials.size() == 0 ? _defaultMaterial->getName() : materials[0]->getName();
		bool includeDefaultMaterial = materials.size() == 0;

		for (Object3D *object : objects) {
			const Mesh &mesh = object->getMesh();

			// Nom
			std::string name = std::string("mesh") + std::to_string(meshCount);
			objstream << "o " << name << std::endl;

			//Materiau
			objstream << "usemtl ";
			std::string materialName = object->getMaterialID();

			if (materialName == "") {
				objstream << defaultMaterialName;
			} else if (materialName == _defaultMaterial->getName()) {
				includeDefaultMaterial = true;
			} else {
				objstream << materialName;
			}
			objstream << std::endl;

			int verticesOffset = verticesRead;
			int normalOffset = normalRead;
			int texCoordOffset = texCoordRead;

			// Ecriture des vertices
			const std::vector<Vertex> &vertList = mesh.getVertices();

			for (const Vertex &vert : vertList) {
				verticesRead++;
				objstream << "v";
				writeValues(objstream, vert.getPosition());
			}

			// Ecriture des normales
			for (const Vertex &vert : vertList) {
				normalRead++;
				objstream << "vn";
				writeValues(objstream, vert.getNormal());
			}

			// Ecriture des coordonnées de texture
			for (const Vertex &vert : vertList) {
				texCoordRead++;
				objstream << "vt";
				writeValues(objstream, vert.getTexture());
			}

			// Ecriture des faces
			const std::vector<Face> &faceList = mesh.getFaces();

			for (const Face &face : faceList) {
				objstream << "f";

				const int vertexCount = face.vertexCount();

				/*/ This code enables to implement optionnal indices easily, despite the fact that it's not yet supported
				const vector<int> vertIndices = face.getIDs(),
						texCoordIndices = face.getIDs(),
						normalIndices = face.getIDs();

				for (int i = 0; i < vertexIndices.size(); i++) {
					objstream << " " << vertIndices[i] + verticesOffset + 1 << "/";
					if (i < texCoordIndices.size() && texCoordIndices[i] != -1 && texCoordIndices[i] < texCoordRead) {
						objstream << texCoordIndices[i] + texCoordOffset + 1;
					}
					objstream << "/";
					if (i < normalIndices.size() && normalIndices[i] != -1 && normalIndices[i] < normalRead) {
						objstream << normalIndices[i] + normalOffset + 1;
					}
				}//*/

				for (int i = 0; i < vertexCount; i++) {
					objstream << " " << face.getID(i) + verticesOffset + 1 << "/" 
						<< face.getID(i) + texCoordOffset + 1 << "/" 
						<< face.getID(i) + normalOffset + 1;
				}
				objstream << std::endl;
			}

			meshCount++;
		}


		// ===== Ajout des matériaux
		// On vérifie que le nom du "default material" n'est pas déjà pris
		if (includeDefaultMaterial) {
			for (std::shared_ptr<Material> &material : materials) {
				if (material->getName() == _defaultMaterial->getName()) {
					includeDefaultMaterial = false;
					break;
				}
			}
		}

		if (includeDefaultMaterial) {
			materials.push_back(_defaultMaterial);
		}

		int materialID = 1;

		for (std::shared_ptr<Material> material : materials) {
			//Déclaration du materiau
			mtlstream << std::endl << "newmtl ";
			std::string materialName = trimSpaces(material->getName());

			if (materialName == "") {
				materialName = "material" + std::to_string(materialID);
				materialID++;
			}
			mtlstream << materialName << std::endl;

			//Kd, Ka, Ks
			auto writeColor = [&mtlstream = mtlstream](const std::string &name, const Color4d& value) {
				mtlstream << name << " " << value._r << " " << value._g << " " << value._b << std::endl;
			};
			writeColor("Kd", material->getKd());
			writeColor("Ka", material->getKa());
			writeColor("Ks", material->getKs());

			//maps
			auto writeMap = [&mtlstream = mtlstream](const std::string &name, const std::string &value) {
				if (value != "") {
					mtlstream << name << " " << value << std::endl;
				}
			};
			writeMap("map_Kd", material->getMapKd());
		}

		if (includeDefaultMaterial) {
			materials.pop_back();
		}
	}

///PRIVATE : méthode permettant d'agrandir un vector avant d'accéder à l'élément
///@returns Une référence vers l'élement à l'emplacement <tt>index</tt>
	template<typename T>
	T &extAt(std::vector<T> &vector, int index) {
		while (vector.size() <= index) {
			vector.push_back(0);
		}

		return vector.at(index);
	}
}
