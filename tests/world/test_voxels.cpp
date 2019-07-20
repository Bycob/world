#include <catch/catch.hpp>

#include <world/core.h>

using namespace world;

TEST_CASE("Voxels basics", "[voxels]") {
    VoxelField voxels(3, 4, 5);

    REQUIRE(voxels.dims() == vec3u{3, 4, 5});
    REQUIRE(voxels.count() == 60);

    SECTION("Initialisation & fill") {
        VoxelField voxels2({3, 3, 3}, -1);

        for (int i = 0; i < voxels2.count(); ++i) {
            double v = voxels2.values()[i];

            if (v != Approx(-1)) {
                FAIL("Value " + std::to_string(i) + " is " + std::to_string(v) +
                     " != -1");
            }
        }

        voxels2.fill(2);

        for (int i = 0; i < voxels2.count(); ++i) {
            double v = voxels2.values()[i];

            if (v != Approx(2)) {
                FAIL("Value " + std::to_string(i) + " is " + std::to_string(v) +
                     " != 2");
            }
        }
    }

    voxels.fill(-1);

    SECTION("Access") {
        voxels.at({1, 2, 3}) = 5;

        CHECK(voxels.at(1, 2, 3) == Approx(5));

        SECTION("Shallow copy") {
            VoxelField shallowVoxels = voxels;
            CHECK(shallowVoxels.dims() == voxels.dims());
            CHECK(shallowVoxels.at({1, 2, 3}) == Approx(5));
            shallowVoxels.at(1, 2, 4) = 4;
            CHECK(voxels.at(1, 2, 4) == Approx(4));
        }

        SECTION("Deep copy") {
            VoxelField deepVoxels = voxels.copy();
            CHECK(deepVoxels.dims() == voxels.dims());
            CHECK(deepVoxels.at(1, 2, 3) == Approx(5));
            deepVoxels.at(1, 2, 4) = 4;
            CHECK_FALSE(voxels.at(1, 2, 4) == Approx(4));
        }
    }
}

TEST_CASE("Voxel2mesh", "[voxels]") {
    SECTION("Marching cubes cases") {
        class VoxelPolicy : public VoxelField {
        public:
            VoxelPolicy(int init) : VoxelField({3}, init) {}
            u8 getPolicy(vec3u at) const { return getMarchingCubePolicy(at); }
        };

        VoxelPolicy v1(1);
        CHECK(v1.getPolicy({0, 0, 0}) == 255);
        VoxelPolicy v2(0);
        CHECK(v2.getPolicy({0, 0, 0}) == 0);
    }

    SECTION("Mesh creation") {
        Mesh mesh;
        VoxelField voxels({3, 3, 3}, -1);

        SECTION("Empty") {
            voxels.fillMesh(mesh);

            CHECK(mesh.getVerticesCount() == 0);
            CHECK(mesh.getFaceCount() == 0);
        }

        SECTION("Octaedron") {
            voxels.at(1, 1, 1) = 1;
            voxels.fillMesh(mesh);

            CHECK(mesh.getVerticesCount() == 6);
            CHECK(mesh.getFaceCount() == 8);
        }

        SECTION("Alternate") {
            // Density is alternatively 1 and -1, so each edge should have a
            // vertex on it
            for (u32 z = 0; z < 3; ++z) {
                for (u32 y = 0; y < 3; ++y) {
                    for (u32 x = 0; x < 3; ++x) {
                        voxels.at(x, y, z) = (x + y + z) % 2 ? 1 : -1;
                    }
                }
            }

            voxels.fillMesh(mesh);
            CHECK(mesh.getVerticesCount() == 54);
            // 54 = number of x aligned edges + number of y aligned edges +
            // number of z aligned edges
        }
    }
}