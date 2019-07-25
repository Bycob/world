#include <catch/catch.hpp>

#include <world/core.h>

using namespace world;

TEST_CASE("Scene", "[assets]") {
    Scene scene;

    SceneNode node;
    node.setPosition({5, 6, 7});

    Mesh mesh;

    SECTION("No names") {
        scene.addMeshNode(node, mesh);

        CHECK(scene.getNodes().size() == 1);
        std::string meshID = scene.getNodes()[0]->getMeshID();
        CHECK(meshID != "");
        CHECK(scene.hasMesh(meshID));
    }

    SECTION("Named") {
        mesh = Mesh("aloha");
        scene.addMeshNode(node, mesh);

        CHECK(scene.getNodes().size() == 1);
        std::string meshID = scene.getNodes()[0]->getMeshID();
        CHECK(meshID == "aloha");
        CHECK(scene.hasMesh(meshID));
    }

    SECTION("Automatic naming") {
        scene.addMesh(Mesh("mesh0"));
        scene.addMesh("mesh1", mesh);
        scene.addMeshNode(node, mesh);
        scene.addMeshNode(node, mesh);

        CHECK(scene.meshCount() == 4);
        std::string mesh0 = "mesh0";
        std::string mesh1 = "mesh1";
        std::string mesh2 = scene.getNodes()[0]->getMeshID();
        std::string mesh3 = scene.getNodes()[1]->getMeshID();
        CHECK(mesh0 != mesh2);
        CHECK(mesh0 != mesh3);
        CHECK(mesh1 != mesh2);
        CHECK(mesh1 != mesh3);
        CHECK(mesh2 != mesh3);
        CHECK(mesh0 == "mesh0");
        CHECK(mesh1 == "mesh1");

        for (auto name : std::vector<std::string>{mesh0, mesh1, mesh2, mesh3}) {
            CHECK(scene.hasMesh(name));
        }
    }
}
