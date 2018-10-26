#include "Collector.h"

namespace world {

Collector::Collector() {}

void Collector::reset() {}

void Collector::fillScene(Scene &scene) {
    /*for (auto it = iterateItems(); it.hasNext(); ++it) {
    const auto &item = (*it).second;

    auto object = item->getObject3D();

    // Material
    std::string materialID = object.getMaterialID();
    auto material = item->getMaterial(materialID);

    if (material) {
    Material addedMat = *material;

    // Change the name to have distinct materials
    // TODO we should not have to do that
    materialID = ItemKeys::toString((*it).first) + materialID;
    addedMat.setName(materialID);
    object.setMaterialID(materialID);

    // Textures
    auto texture = item->getTexture(material->getMapKd());
    if (texture) {
    scene.addTexture(texture->_uid, texture->_image);
    // TODO we should not have to change the uid.
    addedMat.setMapKd(texture->_uid);
    }

    scene.addMaterial(addedMat);
    }

    scene.addObject(object);
    }*/
}

ICollectorChannelBase &Collector::getChannelByType(size_t type) {
    return *_channels.at(type);
}

bool Collector::hasChannelByType(size_t type) const {
    return _channels.find(type) != _channels.end();
}

} // namespace world
