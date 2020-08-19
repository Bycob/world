#include "ObjectNode.h"

namespace world {

// ===== ObjectLod

ObjectLod::ObjectLod(u32 id, double resolution, size_t meshCount)
        : _id(id), _resolution(resolution) {
    addMeshes(meshCount);
}

std::string ObjectLod::getId(size_t i) const {
    return std::to_string(_id) + "." + std::to_string(i);
}

void ObjectLod::collect(Template &tp, ICollector &collector,
                        const ExplorationContext &ctx) const {
    if (collector.hasChannel<Mesh>()) {
        auto &meshChan = collector.getChannel<Mesh>();

        Template::Item item(_resolution);

        for (size_t i = 0; i < _meshes.size(); ++i) {
            SceneNode node = _nodes[i];

            ItemKey key{getId(i)};
            meshChan.put(key, _meshes[i], ctx);
            node.setMesh(ctx(key).str());

            if (!node.getMaterialID().empty()) {
                // TODO The problem of the "keys" API, in one line:
                // Why do we have to apply so much modifications to the key!
                node.setMaterialID(ctx(node.getMaterialID()).str());
            }

            item.add(node);
        }

        tp.insert(item);
    }

    if (collector.hasChannel<Material>()) {
        auto &matChan = collector.getChannel<Material>();

        for (Material material : _materials) {
            // And again!
            if (!material.getMapKd().empty()) {
                material.setMapKd(ctx(material.getMapKd()).str());
            }
            matChan.put(material.getName(), material, ctx);
        }

        if (collector.hasChannel<Image>()) {
            auto &texChan = collector.getChannel<Image>();

            for (size_t i = 0; i < _textures.size(); ++i) {
                texChan.put({getId(i)}, _textures[i], ctx);
            }
        }
    }
}

void ObjectLod::addMeshes(size_t count) {
    for (size_t i = 0; i < count; ++i) {
        _nodes.emplace_back();
        _meshes.emplace_back();
    }
}

Mesh &ObjectLod::addMesh() {
    addMeshes(1);
    return _meshes.back();
}

Mesh &ObjectLod::getMesh(size_t i) { return _meshes.at(i); }

SceneNode &ObjectLod::getNode(size_t i) { return _nodes.at(i); }

void ObjectLod::addMaterials(size_t count) {
    for (size_t i = 0; i < count; ++i) {
        _materials.emplace_back(getId(_materials.size()));
    }
}

Material &ObjectLod::addMaterial() {
    addMaterials(1);
    return _materials.back();
}

Material &ObjectLod::getMaterial(size_t i) { return _materials.at(i); }

void ObjectLod::addTextures(size_t count) {
    for (size_t i = 0; i < count; ++i) {
        _textures.emplace_back(1, 1, ImageType::RGB);
    }
}

Image &ObjectLod::addTexture(int width, int height, ImageType imgType) {
    _textures.emplace_back(width, height, imgType);
    return _textures.back();
}

Image &ObjectLod::getTexture(size_t i) { return _textures.at(i); }


// ===== ObjectNode

ObjectInstance::ObjectInstance(const vec3d &pos) : _pos(pos) {}

ObjectLod &ObjectInstance::addLod(double resolution, size_t meshCount) {
    // TODO sort lods by resolution
    _lods.emplace_back(_lods.size(), resolution, meshCount);
    return _lods.back();
}

ObjectLod &ObjectInstance::getLod(size_t i) { return _lods.at(i); }

ObjectLod &ObjectInstance::getLodByResolution(double res) {
    size_t id;

    for (id = 1; id < _lods.size(); ++id) {
        if (_lods[id]._resolution > res) {
            break;
        }
    }

    return _lods[id - 1];
}

Template ObjectInstance::collect(ICollector &collector,
                                 const ExplorationContext &ctx,
                                 double maxRes) const {
    Template result;

    for (auto &lod : _lods) {
        if (lod._resolution > maxRes)
            break;
        lod.collect(result, collector, ctx);
    }

    result._position = _pos;

    return result;
}
} // namespace world