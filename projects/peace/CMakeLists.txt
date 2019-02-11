
message("-- Build Peace C Plugin")

add_library(cworld SHARED csrc/world.cpp)

target_link_libraries(cworld world)

set(ASSETS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Assets/Plugins)
add_custom_command(TARGET cworld POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:world> ${ASSETS_DIR})
add_custom_command(TARGET cworld POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:cworld> ${ASSETS_DIR})
