
message("-- Build Peace Shared Library")

add_library(peace SHARED
        csrc/world.cpp
        csrc/collector.cpp
        csrc/assets.cpp)

target_link_libraries(peace world)

set(ASSETS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Assets/Peace/Plugins)
add_custom_command(TARGET peace POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:world> ${ASSETS_DIR})
add_custom_command(TARGET peace POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:peace> ${ASSETS_DIR})
