
if (${WORLD_BUILD_TESTS})
    Message("-- Build tests")

    add_executable(test_terrain test_terrain.cpp)
    target_link_libraries(test_terrain worldapi)

    add_executable(test_tree test_tree.cpp)
    target_link_libraries(test_tree worldapi)

    add_executable(test_reliefmap test_reliefmap.cpp)
    target_link_libraries(test_reliefmap worldapi)

    add_executable(test_world test_world.cpp)
    target_link_libraries(test_world worldapi)
endif()

add_subdirectory(worldapi)