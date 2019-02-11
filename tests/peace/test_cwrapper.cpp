#include <catch/catch.hpp>

#include <world/core.h>
#include <world/tree.h>
#include <worldnet/world.h>

using namespace world;

TEST_CASE("normal wrapper use case", "[wrapper]") {
	/*std::unique_ptr<World> world(World::createDemoWorld());
	FirstPersonExplorer explorer;
	explorer.setFarDistance(4000);

	Collector collector;
	collector.addStorageChannel<Object3D>();

	explorer.exploreAndCollect(*world, collector);*/
	
}

TEST_CASE("get mesh vertices", "[wrapper]") {
	Mesh mesh;
	mesh.addVertex(Vertex({ 0, 0, 0 }));
	mesh.addVertex(Vertex({ 1, 0, 0 }));
	mesh.addVertex(Vertex({ 1, 0, 1 }));
	mesh.addVertex(Vertex({ 0, 0, 1 }));

	int faces[][3] = { {0, 1, 2}, {0, 2, 3} };
	mesh.addFace(Face(faces[0]));
	mesh.addFace(Face(faces[1]));

	REQUIRE(meshGetVerticesSize(&mesh) == 8 * 4);

	double vertices[8 * 4];
	meshGetVertices(&mesh, vertices);
	
	for (int i = 0; i < 32; ++i) {
		double vertex = vertices[i];
		INFO(std::to_string(i) + " is " + std::to_string(vertex));
		CHECK((abs(vertex - 1) < 0.000001 || abs(vertex - 0) < 0.000001));
	}

	REQUIRE(meshGetIndiceCount(&mesh) == 3 * 2);
}