
# TODO these are not tests, but more play-around with the API -> move elsewhere ?

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
    
    if (WORLD_BUILD_VULKAN_MODULES AND WORLD_HAS_VULKAN)
        add_executable(test_vulkan test_vulkan.cpp)
        target_link_libraries(test_vulkan worldapi vkworld)
    endif()
endif()

add_subdirectory(worldapi)