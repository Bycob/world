
if (${WORLD_USE_OPENCV})
    message("-- Use OpenCV")
    find_package(OpenCV REQUIRED)

    if(${OpenCV_FOUND})
        include_directories(${OpenCV_INCLUDE_DIRS})
        add_definitions(-DUSE_OPENCV)
    endif()
endif()

if (${WORLD_BUILD_WORLD3D})
    find_package(Irrlicht REQUIRED)
    include_directories(${IRRLICHT_INCLUDE_DIRS})
endif()