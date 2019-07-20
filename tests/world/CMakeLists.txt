
if (${WORLD_BUILD_TESTS})

    set(WORLD_TESTS_SOURCES
            run_test.cpp
            test_chunksystem.cpp
            test_collector.cpp
            test_diamond_square.cpp
            test_heightmap_ground.cpp
            test_image.cpp
            test_math.cpp
            test_memory.cpp
            test_mesh.cpp
            test_nodes.cpp
            test_perlin.cpp
            test_scene.cpp
            test_types.cpp
            test_terrain.cpp
            test_utilities.cpp
            test_voxels.cpp)

    add_executable(runtests ${WORLD_TESTS_SOURCES})
    target_link_libraries(runtests world)
endif()