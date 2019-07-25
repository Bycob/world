#include "Collector.h"

namespace world {

Collector::Collector(CollectorPresets preset) {
    switch (preset) {
    case CollectorPresets::SCENE:
        addStorageChannel<SceneNode>();
        addStorageChannel<Mesh>();
        addStorageChannel<Material>();
        addStorageChannel<Image>();
        break;
    case CollectorPresets ::NONE:
        break;
    }
}

void Collector::reset() {
    for (auto &entry : _channels) {
        entry.second->reset();
    }
}

Scene Collector::toScene() {
    Scene scene;
    fillScene(scene);
    return scene;
}

void Collector::fillScene(Scene &scene) {
    if (hasStorageChannel<SceneNode>() && hasStorageChannel<Mesh>()) {
        auto &objectChannel = getStorageChannel<SceneNode>();
        auto &meshChannel = getStorageChannel<Mesh>();

        if (hasStorageChannel<Material>()) {
            auto &materialChannel = getStorageChannel<Material>();

            if (hasStorageChannel<Image>()) {
                auto &textureChannel = getStorageChannel<Image>();

                for (auto texture : textureChannel) {
                    scene.addTexture(texture._key.str() + ".png",
                                     texture._value);
                }
            }

            for (auto material : materialChannel) {
                Material addedMat(material._value);

                // If texture is "" then the material has no diffuse texture
                if (addedMat.getMapKd() != "")
                    addedMat.setMapKd(addedMat.getMapKd() + ".png");

                scene.addMaterial(material._key.str(), addedMat);
            }
        }

        for (auto object : objectChannel) {
            scene.addNode(object._value);
        }

        for (auto mesh : meshChannel) {
            scene.addMesh(mesh._key.str(), mesh._value);
        }
    }
}

ICollectorChannelBase &Collector::getChannelByType(size_t type) {
    return *_channels.at(type);
}

bool Collector::hasChannelByType(size_t type) const {
    return _channels.find(type) != _channels.end();
}

} // namespace world
