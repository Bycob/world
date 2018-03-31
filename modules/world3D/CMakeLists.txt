
# Recupération des sources
file(GLOB
        WORLD3D_SOURCES
        ./*.*)

# Ajout de la target
if(${WORLD_BUILD_WORLD3D})
	if (${WORLD_HAS_IRRLICHT})
		message("-- Build World3D")
		add_executable(world3D ${WORLD3D_SOURCES})
		target_link_libraries(world3D ${IRRLICHT_LIBRARIES} worldapi)
	else()
		message("-- Cannot build World3D : Missing dependencies (Irrlicht)")
	endif()
endif()

add_subdirectory(tests)
