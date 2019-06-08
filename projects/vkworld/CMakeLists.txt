if (${WORLD_BUILD_VULKAN_MODULES})
    if (${WORLD_HAS_VULKAN})
        message(STATUS "Build Vulkan modules")
        file(GLOB_RECURSE
                VKWORLD_SOURCES
                ./*)

        add_library(vkworld ${WORLD_BUILD_MODE} ${VKWORLD_SOURCES})

        target_link_libraries(vkworld world ${Vulkan_LIBRARIES} ${THIRD_PARTY_LIBRARIES})

        add_subdirectory(shaders/)
    else()
        message(STATUS "Cannot build Vulkan modules : Missign dependencies (Vulkan)")
    endif()
endif()
