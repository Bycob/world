
#Récuperation des sources
file(GLOB_RECURSE
	WORLD_SOURCES
	./*)

# Visual studio source groups
assign_source_group(${WORLD_SOURCES})

# Register target
add_library(world ${WORLD_BUILD_MODE} ${WORLD_SOURCES})

# External libraries
# TODO compile_definitions -> global ? (Or explain why not)
# TODO avoid duplicates in below code
if (${WORLD_BUILD_OPENCV_MODULES})
	if (${WORLD_HAS_OPENCV})
		message("-- Build OpenCV modules")
		target_link_libraries(world ${OpenCV_LIBS})
        target_compile_definitions(world PRIVATE WORLD_BUILD_OPENCV_MODULES=1)
	else()
		message("-- Cannot build OpenCV modules : Missing dependencies (OpenCV)")
	endif()
endif()

# Register target
target_link_libraries(world ${THIRD_PARTY_LIBRARIES})
target_include_directories(world PUBLIC ${THIRD_PARTY_INCLUDES})

# Install target
install(TARGETS world DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/world)

function(world_install_headers directory)
	file(GLOB
			INCLUDES
			${directory}/*.h)
	list(REMOVE_ITEM INCLUDES *_p.h)
	install(FILES ${INCLUDES} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/world/world/${directory}/)
endfunction()

world_install_headers(".")
world_install_headers("assets")
world_install_headers("core")
world_install_headers("flat")
world_install_headers("math")
world_install_headers("terrain")
world_install_headers("tree")
