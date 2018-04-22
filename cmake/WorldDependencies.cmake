
# External dependencies :
# For each dependency a variable WORLD_FOUND_[NAME] is created, indicating if we can use this dependency or not
# - OpenCV

set(WORLD_HAS_OPENCV OFF)

if (${WORLD_BUILD_OPENCV_MODULES})
    find_package(OpenCV QUIET)

    if(${OpenCV_FOUND})
        message("-- Found OpenCV : $OpenCV_LIBRARIES")
        include_directories(${OpenCV_INCLUDE_DIRS})
    else()
        message("-- Could not find OpenCV")
    endif()

    set(WORLD_HAS_OPENCV ${OpenCV_FOUND})
endif()

# - Irrlicht

set(WORLD_HAS_IRRLICHT OFF)

if (${WORLD_BUILD_WORLD3D})
    find_package(Irrlicht QUIET)

    if (${IRRLICHT_FOUND})
        include_directories(${IRRLICHT_INCLUDE_DIRS})
    else()
        message("-- Could not find Irrlicht")
    endif()

    set(WORLD_HAS_IRRLICHT ${IRRLICHT_FOUND})
endif()

# - Vulkan

set(WORLD_HAS_VULKAN OFF)

if (${WORLD_BUILD_VULKAN_MODULES})
    # Find vulkan
    find_library(Vulkan_LIBRARIES
        NAMES
            vulkan-1
        PATH_SUFFIXES
            Lib/
            Lib32/
            lib/
    )

    find_path(Vulkan_INCLUDE_DIRS
        NAMES
            vulkan.hpp
        PATH_SUFFIXES
            Include
            Include/vulkan
            include/
    )

    find_file(Vulkan_GLSL_VALIDATOR
        NAMES
            glslangValidator.exe
            glslangValidator
        PATH_SUFFIXES
            Bin/
            Bin32/
            bin/
    )

    set(Vulkan_FOUND OFF)
    if (Vulkan_LIBRARIES AND NOT (Vulkan_INCLUDE_DIRS STREQUAL "Vulkan_INCLUDE_DIRS-NOTFOUND"))
        set(Vulkan_FOUND ON)
    endif()

    # Configuring project
    if (Vulkan_FOUND)
        message("-- Found Vulkan")
        include_directories(${Vulkan_INCLUDE_DIRS})
    else()
        message("-- Could not find Vulkan")
    endif()

    set(WORLD_HAS_VULKAN ${Vulkan_FOUND})
endif()

message("--")
