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

struct PEACE_EXPORT FirstPersonView {
    double x, y, z;
    double eyeResolution;
    double maxDistance;
};

struct PEACE_EXPORT ZoneView {
    BBox bbox;
    double resolution;
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
const int TERRAIN_CHANNEL = 4;

PEACE_EXPORT CollectorPtr createCollector(int preset) {
    return new Collector(static_cast<CollectorPresets>(preset));
}

PEACE_EXPORT void freeCollector(CollectorPtr collectorPtr) {
    delete static_cast<Collector *>(collectorPtr);
}

PEACE_EXPORT void collectFirstPerson(CollectorPtr collectorPtr,
                                     WorldPtr worldPtr,
                                     ::FirstPersonView view) {
    auto *collector = static_cast<Collector *>(collectorPtr);
    auto *world = static_cast<World *>(worldPtr);
    collector->reset();

    world::FirstPersonView fpsView{view.eyeResolution};
    fpsView.setPosition({view.x, view.y, view.z});
    fpsView.setFarDistance(view.maxDistance);

    try {
        world->collect(*collector, fpsView);
    } catch (std::runtime_error &e) {
        std::cerr << e.what() << std::endl;
        throw;
    }
}

PEACE_EXPORT void collectZone(CollectorPtr collectorPtr, WorldPtr worldPtr,
                              ZoneView view) {
    auto *collector = static_cast<Collector *>(collectorPtr);
    auto *world = static_cast<World *>(worldPtr);
    collector->reset();

    ConstantResolution zoneView{view.resolution};
    zoneView.setBounds({{view.bbox.xmin, view.bbox.ymin, view.bbox.zmin},
                        {view.bbox.xmax, view.bbox.ymax, view.bbox.zmax}});

    try {
        world->collect(*collector, zoneView);
    } catch (std::runtime_error &e) {
        std::cerr << e.what() << std::endl;
        throw;
    }
}

PEACE_EXPORT int collectorGetChannelSize(CollectorPtr collectorPtr, int type) {
    auto *collector = static_cast<Collector *>(collectorPtr);
    // TODO check that channel exists before returning the size.
    switch (type) {
    case NODE_CHANNEL:
        return collector->getStorageChannel<SceneNode>().size();
    case MESH_CHANNEL:
        return collector->getStorageChannel<Mesh>().size();
    case MATERIAL_CHANNEL:
        return collector->getStorageChannel<Material>().size();
    case TEXTURE_CHANNEL:
        return collector->getStorageChannel<Image>().size();
    case TERRAIN_CHANNEL:
        return collector->getStorageChannel<Terrain>().size();
    default:
        return -1;
    }
}

PEACE_EXPORT void collectorGetChannel(CollectorPtr collectorPtr, int type,
                                      char **names, void **objects) {

    auto *collector = static_cast<Collector *>(collectorPtr);
    // TODO check that channel exists before getting its content.
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
    case TERRAIN_CHANNEL:
        getChannelContent(collector->getStorageChannel<Terrain>(), names,
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
