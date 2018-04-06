
if (${WORLD_BUILD_TESTS})

    set(WORLDAPI_TESTS_SOURCES
            run_test.cpp
            test_chunksystem.cpp
            test_collector.cpp
            test_image.cpp
            test_math.cpp
            test_memory.cpp
            test_mesh.cpp
            test_perlin.cpp
            test_types.cpp
            test_terrain.cpp)

    add_executable(runtests ${WORLDAPI_TESTS_SOURCES})
    target_link_libraries(runtests worldapi)
endif()