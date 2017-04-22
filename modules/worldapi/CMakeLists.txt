
# Récuperation des sources
file(GLOB_RECURSE
	WORLD_API_SOURCES
	./*)

assign_source_group(${WORLD_API_SOURCES})

# Ajout de la lib
if(${WORLD_BUILD_STATIC})
	set(WORLD_BUILD_MODE "STATIC")
	message("-- Build static library")
else()
	set(WORLD_BUILD_MODE "SHARED")
	message("-- Build dynamic library")
endif()

add_library(worldapi ${WORLD_BUILD_MODE} ${WORLD_API_SOURCES})
target_link_libraries(worldapi ${OpenCV_LIBS})