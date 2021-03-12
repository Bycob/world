if (${WORLD_BUILD_VULKAN_MODULES})
    if (${WORLD_HAS_VULKAN})
        message(STATUS "Build Vulkan modules")
        file(GLOB_RECURSE
                VKWORLD_SOURCES
                ./*)

        add_library(vkworld ${WORLD_BUILD_MODE} ${VKWORLD_SOURCES})

        target_link_libraries(vkworld world ${Vulkan_LIBRARIES} ${THIRD_PARTY_LIBRARIES} shaderlib)
        rclib_include_directory(shaderlib)

        add_subdirectory(shaders/)

        # TODO add vkworld to install
    else()
        message(STATUS "Cannot build Vulkan modules : Missign dependencies (Vulkan)")
    endif()
endif()
