
if (${WORLD_USE_OPENCV})
    message("-- Use OpenCV")
    find_package(OpenCV REQUIRED)
    include_directories(${OpenCV_INCLUDE_DIRS})
endif()

if (${WORLD_BUILD_WORLD3D})
    find_package(Irrlicht REQUIRED)
    include_directories(${IRRLICHT_INCLUDE_DIRS})
endif()