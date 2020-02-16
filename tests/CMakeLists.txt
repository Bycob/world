
# TODO these are not tests, but more play-around with the API -> move elsewhere ?

if (${WORLD_BUILD_TESTS})
    Message(STATUS "Build tests")

    add_executable(test_terrain test_terrain.cpp)
    target_link_libraries(test_terrain world)

    add_executable(test_tree test_tree.cpp)
    target_link_libraries(test_tree world)

    add_executable(test_reliefmap test_reliefmap.cpp)
    target_link_libraries(test_reliefmap world)

    add_executable(test_world test_world.cpp)
    target_link_libraries(test_world world)
    
    if (WORLD_BUILD_VULKAN_MODULES AND WORLD_HAS_VULKAN)
        add_executable(test_vulkan test_vulkan.cpp)
        target_link_libraries(test_vulkan world vkworld)

        target_link_libraries(test_world vkworld)
        target_compile_definitions(test_world PRIVATE USE_VKWORLD=1)
    endif()

    if (WORLD_COMPILER_IS_GNU)
        add_executable(mem_test mem_test.cpp)
        target_link_libraries(mem_test world)
    endif()
endif()

add_subdirectory(world)
add_subdirectory(peace)