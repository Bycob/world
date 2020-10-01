
if (${WORLD_BUILD_PEACE})
    message("-- Build Peace Shared Library")

    add_library(peace SHARED
            csrc/world.cpp
            csrc/collector.cpp
            csrc/assets.cpp
            csrc/util.cpp
            csrc/terrains.cpp)

    target_link_libraries(peace world)

    # TODO Use a list of libraries instead of checking for all projects ?
    if (${WORLD_BUILD_VULKAN_MODULES} AND ${WORLD_HAS_VULKAN})
        message(STATUS "Build Peace with VkWorld support")
        target_compile_definitions(peace PRIVATE USE_VKWORLD=1)
        target_link_libraries(peace vkworld)
    endif()

    set(ASSETS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Assets/Peace/Plugins)
    add_custom_command(TARGET peace POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:world> ${ASSETS_DIR})
    add_custom_command(TARGET peace POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:peace> ${ASSETS_DIR})

    # TODO Disable dependency copying when building in STATIC mode
    if (${WORLD_BUILD_VULKAN_MODULES} AND ${WORLD_HAS_VULKAN})
        add_custom_command(TARGET peace POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:vkworld> ${ASSETS_DIR})
    endif()
endif()
