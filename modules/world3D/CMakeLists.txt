
# Recupération des sources
file(GLOB_RECURSE
        WORLD3D_SOURCES
        ./*)

# Ajout de la target
if(${WORLD_BUILD_WORLD3D})
    message("-- Build World3D")
    add_executable(world3D ${WORLD3D_SOURCES})
    target_link_libraries(world3D ${IRRLICHT_LIBRARIES} worldapi)
endif()
