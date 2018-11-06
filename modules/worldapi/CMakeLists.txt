
#Récuperation des sources
file(GLOB_RECURSE
	WORLD_API_SOURCES
	./*)

# Visual studio source groups
assign_source_group(${WORLD_API_SOURCES})

# Register target
if(${WORLD_STATIC_BUILD})
	set(WORLD_BUILD_MODE "STATIC")
	message("-- Build WorldAPI as a static library")
else()
	set(WORLD_BUILD_MODE "SHARED")
	message("-- Build WorldAPI as a dynamic library")
endif()

add_library(worldapi ${WORLD_BUILD_MODE} ${WORLD_API_SOURCES})

# External libraries
# TODO compile_definitions -> global ? (Or explain why not)
# TODO avoid duplicates in below code
if (${WORLD_BUILD_OPENCV_MODULES})
	if (${WORLD_HAS_OPENCV})
		message("-- Build OpenCV modules")
		target_link_libraries(worldapi ${OpenCV_LIBS})
        target_compile_definitions(worldapi PRIVATE WORLD_BUILD_OPENCV_MODULES=1)
	else()
		message("-- Cannot build OpenCV modules : Missing dependencies (OpenCV)")
	endif()
endif()

if (${WORLD_BUILD_VULKAN_MODULES})
	if (${WORLD_HAS_VULKAN})
		message("-- Build Vulkan modules")
		target_link_libraries(worldapi ${Vulkan_LIBRARIES})
		target_compile_definitions(worldapi PRIVATE WORLD_BUILD_VULKAN_MODULES=1)
	else()
		message("-- Cannot build Vulkan modules : Missign dependencies (Vulkan)")
	endif()
endif()


# Register target
target_link_libraries(worldapi ${THIRD_PARTY_LIBRARIES})
target_include_directories(worldapi PUBLIC ${THIRD_PARTY_INCLUDES})

add_subdirectory(shaders/)