
if (${WORLD_BUILD_TESTS})

    set(WORLDAPI_TESTS_SOURCES
            run_test.cpp
            test_math.cpp
            test_perlin.cpp
            test_types.cpp)

    add_executable(runtests ${WORLDAPI_TESTS_SOURCES})
    target_link_libraries(runtests worldapi)
endif()