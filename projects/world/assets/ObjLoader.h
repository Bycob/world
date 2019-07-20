#pragma once

#include "world/core/WorldConfig.h"

#include <vector>
#include <memory>
#include <set>

#include "Scene.h"

namespace world {

#define DEFAULT_MATERIAL_NAME "default"

/**Cette classe sert � charger les fichiers .obj � l'aide du tiny_obj_loader,
ainsi qu'� les �crire.
Elle fait l'interface entre les fichiers .obj et la classe Mesh. */
class WORLDAPI_EXPORT ObjLoader {
public:
    ObjLoader(bool triangulate = true);

    virtual ~ObjLoader();

    void write(const Scene &scene, std::string filename) const;

    void write(const Scene &scene, std::ostream &objstream,
               std::ostream &mtlstream,
               const std::string &textureFolder = "") const;

    void read(Scene &scene, const std::string &filename) const;

private:
    bool _triangulate;
    Material _defaultMaterial;

    void writeTextures(const Scene &scene, const std::set<std::string> &paths,
                       const std::string &directory) const;
};
} // namespace world
