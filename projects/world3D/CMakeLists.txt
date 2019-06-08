
# Recupération des sources
file(GLOB
        WORLD3D_SOURCES
        ./*.*)

# Ajout de la target
if(${WORLD_BUILD_WORLD3D})
	if (${WORLD_HAS_IRRLICHT})
		message(STATUS "Build World3D")
		add_executable(world3D ${WORLD3D_SOURCES})
		target_link_libraries(world3D ${IRRLICHT_LIBRARIES} world)
	else()
		message(STATUS "Cannot build World3D : Missing dependencies (Irrlicht)")
	endif()
endif()

add_subdirectory(tests)
