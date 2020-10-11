#define CATCH_CONFIG_MAIN
#include <catch/catch.hpp>

#include <world/core.h>
#include <world/tree.h>

#include <peace/csrc/collector.cpp>
#include <peace/csrc/world.cpp>
#include <peace/csrc/assets.cpp>

using namespace world;

TEST_CASE("normal wrapper use case", "[peace][.long]") {
    SECTION("Collect the demo world") {
        World *world = World::createDemoWorld();

        Collector collector(CollectorPresets::SCENE);
        collectFirstPerson(&collector, world, ::FirstPersonView{0, 0, 1000});

        auto chanSize = collector.getStorageChannel<SceneNode>().size();
        char **names = new char *[chanSize];
        void **nodes = new void *[chanSize];
        collectorGetChannel(&collector, NODE_CHANNEL, names, nodes);

        for (size_t i = 0; i < chanSize; ++i) {
            auto node = readNode(nodes[i]);
            CHECK(strlen(node.mesh) > 0);
        }

        delete world;
    }
}

TEST_CASE("Test collector", "[peace]") {}

TEST_CASE("get mesh vertices", "[peace]") {
    Mesh mesh;
    mesh.addVertex(Vertex({0, 0, 0}));
    mesh.addVertex(Vertex({1, 0, 0}));
    mesh.addVertex(Vertex({1, 0, 1}));
    mesh.addVertex(Vertex({0, 0, 1}));

    int faces[][3] = {{0, 1, 2}, {0, 2, 3}};
    mesh.addFace(Face(faces[0]));
    mesh.addFace(Face(faces[1]));

    int vertSize;
    int indicesSize;
    readMeshSizes(&mesh, &vertSize, &indicesSize);

    REQUIRE(vertSize == 8 * 4);
    REQUIRE(indicesSize == 3 * 2);

    double vertices[8 * 4];
    int indices[3 * 2];
    readMesh(&mesh, vertices, indices);

    for (int i = 0; i < 32; ++i) {
        double vertex = vertices[i];
        INFO(std::to_string(i) + " is " + std::to_string(vertex));
        CHECK((abs(vertex - 1) < 0.000001 || abs(vertex - 0) < 0.000001));
    }
}