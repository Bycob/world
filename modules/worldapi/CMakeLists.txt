
find_package(OpenCV REQUIRED)


file(GLOB_RECURSE
	WORLD_API_SOURCES
	./*)

add_library(worldapi SHARED ${WORLD_API_SOURCES})
target_link_libraries(worldapi ${OpenCV_LIBS})

assign_source_group(${WORLD_API_SOURCES})