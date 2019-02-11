#include "world.h"

#include <iostream>
#include <cstring>

#include <world/core.h>

using namespace world;

HANDLE createDemoWorld() {
    return World::createDemoWorld();
}

HANDLE createCollector() {
    Collector *collector = new Collector();
    collector->addStorageChannel<Object3D>();
    return collector;
}

void collect(HANDLE world, HANDLE collector) {
    FirstPersonExplorer explorer;
	explorer.setPosition({ 0, 0, 150 });
    explorer.exploreAndCollect(*static_cast<World*>(world), *static_cast<Collector*>(collector));
}

HANDLE collectorGetChannel(HANDLE collector, int type) {
    return &static_cast<Collector*>(collector)->getChannel<Object3D>();
}

int channelGetObjectsCount(HANDLE channelPtr) {
	auto &channel = *static_cast<CollectorChannel<Object3D>*>(channelPtr);

	u32 i = 0;
	for (auto &item : channel) {
		i++;
	}
	return i;
}

void channelGetObjects(HANDLE channelPtr, HANDLE* objects) {
    auto &channel = *static_cast<CollectorChannel<Object3D>*>(channelPtr);

	u32 i = 0;
    for (auto &item : channel) {
        objects[i] = const_cast<Object3D*>(&item._value);
		i++;
    }
}

HANDLE objectGetMesh(HANDLE objectPtr) {
	auto &object = *static_cast<Object3D*>(objectPtr);
	return const_cast<Mesh*>(&object.getMesh());
}

void objectGetPosition(HANDLE objectPtr, double *position) {
	auto &object = *static_cast<Object3D*>(objectPtr);
	auto posVec = object.getPosition();
	position[0] = posVec.x;
	position[1] = posVec.y;
	position[2] = posVec.z;
}

int meshGetIndiceCount(HANDLE meshPtr) {
	Mesh &mesh = *static_cast<Mesh*>(meshPtr);
	return mesh.getFaceCount() * 3;
}

void meshGetIndices(HANDLE meshPtr, int* indices) {
    Mesh &mesh = *static_cast<Mesh*>(meshPtr);
	
	for (u32 i = 0; i < mesh.getFaceCount(); ++i) {
		Face &face = mesh.getFace(i);
		indices[i * 3 + 0] = face.getID(0);
		indices[i * 3 + 1] = face.getID(1);
		indices[i * 3 + 2] = face.getID(2);
	}
}

#define DOUBLE_VERTEX_SIZE (sizeof(Vertex) / sizeof(double))

// Gives vertices size in double
int meshGetVerticesSize(HANDLE meshPtr) {
	Mesh &mesh = *static_cast<Mesh*>(meshPtr);
	return mesh.getVerticesCount() * DOUBLE_VERTEX_SIZE;
}

void meshGetVertices(HANDLE meshPtr, double* vertices) {
    Mesh &mesh = *static_cast<Mesh*>(meshPtr);
	
	for (u32 i = 0; i < mesh.getVerticesCount(); ++i) {
		Vertex &vertex = mesh.getVertex(i);
		std::memcpy(vertices + i * DOUBLE_VERTEX_SIZE, &vertex, sizeof(Vertex));
	}
}
