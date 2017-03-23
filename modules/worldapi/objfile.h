#pragma once

#include <tiny_obj_loader.h>
#include <vector>
#include <memory>

#include "worldapidef.h"

class Mesh;
class Material;

#define DEFAULT_MATERIAL_NAME "default"

//Cette classe sert � charger les fichiers .obj � l'aide du tiny_obj_loader,
//ainsi qu'� les �crire.
//Elle fait l'interface entre les fichiers .obj et la classe Mesh. 
class WORLDAPI_EXPORT ObjLoader {
public:
	ObjLoader();
	ObjLoader(std::string pathname, bool triangulate = true);
	virtual ~ObjLoader();

	//IO

	void write(std::string filename);
	void write(std::ostream & objstream, std::ostream & mtlstream);

	//Accesseurs

	/** Ajoute le mesh pass� en param�tres � ce fichier .obj. Ainsi le mesh pourra
	�tre �crit dans un fichier avec la m�thode write(filename).*/
	void addMesh(std::shared_ptr<Mesh> mesh);

	/** Ajoute le mesh pass� en param�tres � ce fichier .obj. Le mesh ajout� est
	copi�, ainsi cette m�thode est plus lente que la m�thode addMesh(std::shared_ptr)*/
	void addMesh(const Mesh & mesh);
	void addMaterial(std::shared_ptr<Material> material);

	void getMeshes(std::vector<std::shared_ptr<Mesh>> & output);
	void getMaterials(std::vector<std::shared_ptr<Material>> & output);

	//DEBUG

	void printDebug();

private:
	void read(bool triangulate);

	std::string _filename;

	std::shared_ptr<Material> _defaultMaterial;

	std::vector<std::shared_ptr<Material>> _materials;
	std::vector<std::shared_ptr<Mesh>> _meshes;
};
