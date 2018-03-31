
# External dependencies :
# For each dependency a variable WORLD_FOUND_[NAME] is created, indicating if we can use this dependency or not
# - OpenCV

set(WORLD_HAS_OPENCV OFF)

if (${WORLD_BUILD_OPENCV_MODULES})
    find_package(OpenCV QUIET)

    if(${OpenCV_FOUND})
		message("-- Found OpenCV")
        include_directories(${OpenCV_INCLUDE_DIRS})
    endif()

	set(WORLD_HAS_OPENCV ${OpenCV_FOUND})
endif()

# - Irrlicht

set(WORLD_HAS_IRRLICHT OFF)

if (${WORLD_BUILD_WORLD3D})
    find_package(Irrlicht QUIET)

	if (${IRRLICHT_FOUND})
		include_directories(${IRRLICHT_INCLUDE_DIRS})
	endif()

	set(WORLD_HAS_IRRLICHT ${IRRLICHT_FOUND})
endif()