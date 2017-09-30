#pragma once

#include "worldapidef.h"

#include <vector>
#include <memory>

class Mesh;
class Material;
class Scene;

#define DEFAULT_MATERIAL_NAME "default"

/**Cette classe sert � charger les fichiers .obj � l'aide du tiny_obj_loader,
ainsi qu'� les �crire.
Elle fait l'interface entre les fichiers .obj et la classe Mesh. */
class WORLDAPI_EXPORT ObjLoader {
public:
	ObjLoader(bool triangulate = true);
	virtual ~ObjLoader();

	void write(const Scene & scene, std::string filename) const;
	void write(const Scene & scene, std::ostream & objstream, std::ostream & mtlstream) const;

	Scene * read(const std::string & filename) const;
private:

	bool _triangulate;
	std::shared_ptr<Material> _defaultMaterial;
};
