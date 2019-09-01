#include <catch/catch.hpp>

#include <world/core.h>

using namespace world;

TEST_CASE("MeshOps", "[mesh]") {
    SECTION("Concatenation") {
        Mesh meshes[3];

        for (int i = 0; i < 3; ++i) {
            double j = i * 3;
            meshes[i].newVertex({j, j, j});
            meshes[i].newVertex({j + 1, j, j});
            meshes[i].newVertex({j + 2, j, j});
            meshes[i].newFace(0, 1, 2);
        }

        Mesh result = MeshOps::concatMeshes(meshes[0], meshes[1], meshes[2]);
        REQUIRE(result.getVerticesCount() == 9);
        REQUIRE(result.getFaceCount() == 3);

        CHECK((result.getVertex(8).getPosition() - vec3d{8, 6, 6}).norm() ==
              Approx(0));
        CHECK(result.getFace(2).getID(2) == 8);
        CHECK(result.getFace(1).getID(2) == 5);
    }
}

TEST_CASE("Mesh benchmarks", "[mesh][!benchmark]") {
    Mesh mesh1;
    Mesh mesh2;
    Mesh mesh3;

    SECTION("reservation") {
        BENCHMARK("reserve 100000 vertices") {
            Mesh m;
            m.reserveVertices(100000);
        }
    }

    mesh1.reserveVertices(1000000);
    mesh2.reserveVertices(1000000);
    mesh3.reserveVertices(1000000);

    SECTION("creation") {
        BENCHMARK("create 100000 vertices") {
            for (int i = 0; i < 100000; i++) {
                Vertex &v = mesh1.newVertex();
            }
        }

        BENCHMARK("create 100000 vertices and set values") {
            for (int i = 0; i < 100000; i++) {
                Vertex &v = mesh2.newVertex();
                v.setPosition(1, 1, 1);
                v.setNormal(1, 1, 1);
                v.setTexture(1, 1);
            }
        }

        BENCHMARK("create 100000 vertices with initial values") {
            for (int i = 0; i < 100000; i++) {
                Vertex &v = mesh3.newVertex({1, 1, 1}, {1, 1, 1}, {1, 1});
            }
        }
    }

    for (int i = 0; i < 10000000; i++) {
        mesh1.newVertex();
        mesh2.newVertex();
        mesh3.newVertex();
    }

    SECTION("access") {
        BENCHMARK("access 100000 vertex") {
            for (int i = 0; i < 100000; i++) {
                mesh1.getVertex(i);
            }
        }

        BENCHMARK("set values for 100000 vertices") {
            for (int i = 0; i < 100000; i++) {
                Vertex &v = mesh2.getVertex(i);
                v.setPosition(1, 1, 1);
                v.setNormal(1, 1, 1);
                v.setTexture(1, 1);
            }
        }
    }
}