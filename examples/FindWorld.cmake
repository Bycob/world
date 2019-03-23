
# This script helps CMake to find World
#
# If World is found, following variables are set
# - WORLD_FOUND : true
# - WORLD_LIBRARIES : path to world libraries
# - WORLD_INCLUDE_DIRS : path to world include directories
#

if (WORLD_LIBRARIES AND WORLD_INCLUDE_DIRS)
    set(WORLD_FOUND TRUE)
else(WORLD_LIBRARIES AND WORLD_INCLUDE_DIRS)
    find_library(WORLD_LIBRARY
        NAMES
            world
        PATHS
            /usr/local/lib
            /usr/local/lib/world
        )

    find_path(WORLD_INCLUDE_DIR
        NAMES
            world/core.h
        PATHS
            /usr/local/include
            /usr/local/include/world
        )
    message(${WORLD_INCLUDE_DIR})

    if (WORLD_LIBRARY AND WORLD_INCLUDE_DIR)
        set(WORLD_FOUND TRUE)
        set(WORLD_LIBRARIES ${WORLD_LIBRARY})
        set(WORLD_INCLUDE_DIRS ${WORLD_INCLUDE_DIR})
    else()
        set(WORLD_FOUND FALSE)
    endif()
endif()