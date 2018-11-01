#include "Collector.h"

namespace world {

Collector::Collector() {}

void Collector::reset() {
    for (auto &entry : _channels) {
        entry.second->reset();
    }
}

void Collector::fillScene(Scene &scene) {
    if (hasStorageChannel<Object3D>()) {
        auto &objectChannel = getStorageChannel<Object3D>();

        if (hasStorageChannel<Material>()) {
            auto &materialChannel = getStorageChannel<Material>();

            if (hasStorageChannel<Image>()) {
                auto &textureChannel = getStorageChannel<Image>();

                for (auto texture : textureChannel) {
                    scene.addTexture(str(texture._key) + ".png",
                                     texture._value);
                }
            }

            for (auto material : materialChannel) {
                Material addedMat(material._value);
                addedMat.setMapKd(addedMat.getMapKd() + ".png");

                scene.addMaterial(addedMat);
            }
        }

        for (auto object : objectChannel) {
            scene.addObject(object._value);
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
