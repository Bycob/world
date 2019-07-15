#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <set>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader/tiny_obj_loader.h>

#include "ObjLoader.h"

#include "Mesh.h"
#include "Scene.h"
#include "world/core/StringOps.h"

namespace world {

// TODO delete triangulate parameter (unused)
ObjLoader::ObjLoader(bool triangulate)
        : _defaultMaterial(DEFAULT_MATERIAL_NAME), _triangulate(triangulate) {}

ObjLoader::~ObjLoader() {}

void ObjLoader::read(Scene &scene, const std::string &filename) const {
    // Lecture via tinyobj
    std::string errstr;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    bool success = tinyobj::LoadObj(shapes, materials, errstr, filename.c_str(),
                                    NULL, true);

    if (!success) {
        throw std::ios_base::failure(errstr);
    }

    // Convert to SceneNode
    int meshID = 0;

    for (auto shape : shapes) {
        Mesh mesh;

        // positions
        std::vector<float> &positions = shape.mesh.positions;
        assert(positions.size() % 3 == 0);

        for (u32 i = 0; i < positions.size() / 3; i++) {
            Vertex vert;
            vert.setPosition(positions.at(i * 3), positions.at(i * 3 + 1),
                             positions.at(i * 3 + 2));
            mesh.addVertex(vert);
        }

        // normales
        std::vector<float> &normals = shape.mesh.normals;
        assert(normals.size() % 3 == 0);

        for (u32 i = 0; i < normals.size() / 3; i++) {
            mesh.getVertex(i).setNormal(normals.at(i * 3),
                                        normals.at(i * 3 + 1),
                                        normals.at(i * 3 + 2));
        }

        // texture
        std::vector<float> &textures = shape.mesh.texcoords;
        assert(textures.size() % 2 == 0);

        for (u32 i = 0; i < textures.size() / 2; i++) {
            mesh.getVertex(i).setTexture(textures.at(i * 2),
                                         textures.at(i * 2 + 1));
        }

        // faces
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

        std::string meshName = "mesh" + std::to_string(meshID);
        scene.addMesh(meshName, mesh);
        scene.addNode(SceneNode(meshName));
    }
}

void ObjLoader::write(const Scene &scene, std::string filename) const {
    // Delete extension if it's already in the name
    if (endsWith(filename, ".obj")) {
        filename = filename.substr(0, filename.size() - 4);
    }

    // Get .obj file stream
    std::ofstream objfile(filename + ".obj");
    if (!objfile.is_open()) {
        throw std::ios_base::failure("oh, shit, we didnt wrote u're file, " +
                                     filename +
                                     ".obj... sorry for the disagreement :-*");
    }

    // Get .mtl file stream
    std::ofstream mtlfile(filename + ".mtl");
    if (!mtlfile.is_open()) {
        throw std::ios_base::failure("oh, shit, we didnt wrote u're file, " +
                                     filename +
                                     ".mtl... sorry for the disagreement :-*");
    }

    // Print mtl location in .obj file
    size_t lastSlashIndex = filename.find_last_of("/\\");
    if (lastSlashIndex == std::string::npos) {
        lastSlashIndex = 0;
    }
    std::string localName = filename.substr(lastSlashIndex + 1);
    objfile << "mtllib " << localName << ".mtl" << std::endl;

    // Write
    std::string textureFolder = filename.substr(0, lastSlashIndex + 1);
    write(scene, objfile, mtlfile, textureFolder);

    objfile.close();
    mtlfile.close();
}

// Useful functions
void writeValues(std::ostream &file, const vec3d &vert) {
    file << " " << vert.x << " " << vert.y << " " << vert.z << std::endl;
}

void writeValues(std::ostream &file, const vec2d &vert) {
    file << " " << vert.x << " " << vert.y << std::endl;
}

std::string getTexturePath(const std::string &texID) {
    std::regex e("/");
    return std::regex_replace(texID, e, "_") +
           (endsWith(texID, ".png") ? "" : ".png");
}

void ObjLoader::write(const Scene &scene, std::ostream &objstream,
                      std::ostream &mtlstream,
                      const std::string &textureFolder) const {

    std::vector<SceneNode *> objects = scene.getNodes();

    int meshCount = 1;
    int verticesRead = 0;
    int normalRead = 0;
    int texCoordRead = 0;

    std::string defaultMatName = _defaultMaterial.getName();
    std::set<std::string> materials;

    for (SceneNode *object : objects) {
        const Mesh &mesh = scene.getMesh(object->getMeshID());

        // Name
        std::string name = std::string("mesh") + std::to_string(meshCount);
        objstream << "o " << name << std::endl;

        // Materiau
        objstream << "usemtl ";
        std::string materialName = object->getMaterialID();

        if (materialName == "") {
            materialName = defaultMatName;
        }
        materials.insert(materialName);
        objstream << materialName << std::endl;

        int verticesOffset = verticesRead;
        int normalOffset = normalRead;
        int texCoordOffset = texCoordRead;

        // Ecriture des vertices
        for (u32 vi = 0; vi < mesh.getVerticesCount(); vi++) {
            verticesRead++;
            objstream << "v";
            writeValues(objstream, mesh.getVertex(vi).getPosition() +
                                       object->getPosition());
        }

        // Ecriture des normales
        for (u32 vi = 0; vi < mesh.getVerticesCount(); vi++) {
            normalRead++;
            objstream << "vn";
            writeValues(objstream, mesh.getVertex(vi).getNormal());
        }

        // Ecriture des coordonnées de texture
        for (u32 vi = 0; vi < mesh.getVerticesCount(); vi++) {
            texCoordRead++;
            objstream << "vt";
            writeValues(objstream, mesh.getVertex(vi).getTexture());
        }

        // Ecriture des faces
        for (u32 fi = 0; fi < mesh.getFaceCount(); fi++) {
            objstream << "f";

            const Face &face = mesh.getFace(fi);
            const int vertexCount = face.vertexCount();

            /*/ This code enables to implement optionnal indices easily, despite
            the fact that it's not yet supported const vector<int> vertIndices =
            face.getIDs(), texCoordIndices = face.getIDs(), normalIndices =
            face.getIDs();

            for (int i = 0; i < vertexIndices.size(); i++) {
                    objstream << " " << vertIndices[i] + verticesOffset + 1 <<
            "/"; if (i < texCoordIndices.size() && texCoordIndices[i] != -1 &&
            texCoordIndices[i] < texCoordRead) { objstream << texCoordIndices[i]
            + texCoordOffset + 1;
                    }
                    objstream << "/";
                    if (i < normalIndices.size() && normalIndices[i] != -1 &&
            normalIndices[i] < normalRead) { objstream << normalIndices[i] +
            normalOffset + 1;
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
    bool addDefaultMaterial =
        materials.find(defaultMatName) != materials.end() &&
        !scene.hasMaterial(defaultMatName);
    std::set<std::string> texturePaths;

    for (std::string matName : materials) {
        const Material &material =
            addDefaultMaterial && matName == defaultMatName
                ? _defaultMaterial
                : scene.getMaterial(matName);

        // Déclaration du materiau
        mtlstream << std::endl << "newmtl ";
        matName = trimSpaces(matName);
        mtlstream << matName << std::endl;

        // Kd, Ka, Ks
        auto writeColor = [&mtlstream = mtlstream](const std::string &name,
                                                   const Color4d &value) {
            mtlstream << name << " " << value._r << " " << value._g << " "
                      << value._b << std::endl;
        };
        writeColor("Kd", material.getKd());
        writeColor("Ka", material.getKa());
        writeColor("Ks", material.getKs());

        // maps
        auto writeMap = [&](const std::string &name, const std::string &value) {
            if (value != "") {
                mtlstream << name << " " << getTexturePath(value) << std::endl;
                texturePaths.insert(value);
            }
        };
        writeMap("map_Kd", material.getMapKd());
    }

    if (textureFolder != "") {
        writeTextures(scene, texturePaths, textureFolder);
    }
}

void ObjLoader::writeTextures(const Scene &scene,
                              const std::set<std::string> &paths,
                              const std::string &folder) const {

    for (const std::string &path : paths) {
        if (!path.empty()) {
            const Image &texture = scene.getTexture(path);
            texture.write(folder + getTexturePath(path));
        }
    }
}
} // namespace world
