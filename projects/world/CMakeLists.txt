
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
if(WIN32)
	install(TARGETS world
		LIBRARY DESTINATION lib
		RUNTIME DESTINATION bin
		ARCHIVE DESTINATION staticlib)
	install(DIRECTORY .
		DESTINATION include/world/${directory}/
		FILES_MATCHING REGEX "\\.(h|inl)$"
		PATTERN "*_p.h" EXCLUDE)
else()
	install(TARGETS world DESTINATION lib/world)
	install(DIRECTORY .
		DESTINATION include/world/world/${directory}/
		FILES_MATCHING REGEX "\\.(h|inl)$"
		PATTERN "*_p.h" EXCLUDE)
endif()
