#include "common.h"

#include <world/core.h>

using namespace world;

template <typename T>
inline void getChannelContent(CollectorChannel<T> &channel, char **names,
                              void **objects) {
    int i = 0;
    for (const auto &keyval : channel) {
        names[i] = strdup(keyval._key.str().c_str());
        objects[i] = const_cast<T *>(&keyval._value);
        ++i;
    }
}

extern "C" {

struct PEACE_EXPORT CollectorView {
    double x, y, z;
};

struct PEACE_EXPORT CollectorNode {
    char *mesh;
    char *material;
    double posX, posY, posZ;
    double scaleX, scaleY, scaleZ;
    double rotX, rotY, rotZ;
};

const int NODE_CHANNEL = 0;
const int MESH_CHANNEL = 1;
const int MATERIAL_CHANNEL = 2;
const int TEXTURE_CHANNEL = 3;

PEACE_EXPORT CollectorPtr createCollector() {
    return new Collector(CollectorPresets::SCENE);
}

PEACE_EXPORT void freeCollector(CollectorPtr collectorPtr) {
    delete static_cast<Collector *>(collectorPtr);
}

PEACE_EXPORT void collect(CollectorPtr collectorPtr, WorldPtr worldPtr,
                          CollectorView view) {
    auto *collector = static_cast<Collector *>(collectorPtr);
    collector->reset();
    auto *world = static_cast<World *>(worldPtr);
    FirstPersonView fpsView{};
    fpsView.setPosition({view.x, view.y, view.z});
    world->collect(*collector, fpsView);
}

PEACE_EXPORT int collectorGetChannelSize(CollectorPtr collectorPtr, int type) {
    auto *collector = static_cast<Collector *>(collectorPtr);
    switch (type) {
    case NODE_CHANNEL:
        return collector->getStorageChannel<SceneNode>().size();
    case MESH_CHANNEL:
        return collector->getStorageChannel<Mesh>().size();
    case MATERIAL_CHANNEL:
        return collector->getStorageChannel<Material>().size();
    case TEXTURE_CHANNEL:
        return collector->getStorageChannel<Image>().size();
    default:
        return -1;
    }
}

PEACE_EXPORT void collectorGetChannel(CollectorPtr collectorPtr, int type,
                                      char **names, void **objects) {

    auto *collector = static_cast<Collector *>(collectorPtr);

    switch (type) {
    case NODE_CHANNEL:
        getChannelContent(collector->getStorageChannel<SceneNode>(), names,
                          objects);
        break;
    case MESH_CHANNEL:
        getChannelContent(collector->getStorageChannel<Mesh>(), names, objects);
        break;
    case MATERIAL_CHANNEL:
        getChannelContent(collector->getStorageChannel<Material>(), names,
                          objects);
        break;
    case TEXTURE_CHANNEL:
        getChannelContent(collector->getStorageChannel<Image>(), names,
                          objects);
        break;
    default:
        // Return error
        break;
    }
}

PEACE_EXPORT CollectorNode readNode(SceneNodePtr nodePtr) {
    auto *node = static_cast<SceneNode *>(nodePtr);
    CollectorNode result{};
    result.mesh = const_cast<char *>(node->getMeshID().c_str());
    result.material = const_cast<char *>(node->getMaterialID().c_str());
    vec3d pos = node->getPosition(), scale = node->getScale(),
          rot = node->getRotation();
    result.posX = pos.x;
    result.posY = pos.y;
    result.posZ = pos.z;
    result.scaleX = scale.x;
    result.scaleY = scale.y;
    result.scaleZ = scale.z;
    result.rotX = rot.x;
    result.rotY = rot.y;
    result.rotZ = rot.z;
    return result;
}
}
