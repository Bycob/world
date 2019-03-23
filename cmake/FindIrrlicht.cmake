# - Try to find Irrlicht
# Once done this will define
#
#  IRRLICHT_FOUND - system has Irrlicht
#  IRRLICHT_INCLUDE_DIRS - the Irrlicht include directory
#  IRRLICHT_LIBRARIES - Link these to use Irrlicht
#  IRRLICHT_DEFINITIONS - Compiler switches required for using Irrlicht
#
#  Copyright (c) 2006 Andreas Schneider <mail@cynapses.org>
#  Modified by Louis Jean for the World library
#
#  Redistribution and use is allowed according to the terms of the New
#  BSD license.
#  For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#


if (IRRLICHT_LIBRARIES AND IRRLICHT_INCLUDE_DIRS)
  # in cache already
  set(IRRLICHT_FOUND TRUE)
else (IRRLICHT_LIBRARIES AND IRRLICHT_INCLUDE_DIRS)
  # declare variables for different platforms
  if (WIN32)
    set(IRRLICHT_PATHS
      ${PROJECT_SOURCE_DIR}/../irrlicht/include
    )

    set(IRRLICHT_LIBRARY_PATHS
      ${PROJECT_SOURCE_DIR}/../irrlicht/lib
    )

    if (${WORLD_COMPILER_IS_MSVC})
      set(IRRLICHT_LIBRARY_PATHS ${IRRLICHT_LIBRARY_PATHS}
        ${PROJECT_SOURCE_DIR}/../irrlicht/lib/Win64-visualstudio
      )
    elseif(${WORLD_COMPILER_IS_GNU})
      set(IRRLICHT_LIBRARY_PATHS ${IRRLICHT_LIBRARY_PATHS}
        ${PROJECT_SOURCE_DIR}/../irrlicht/lib/gnu-gcc # TODO à revérifier
      )
    endif()

  else (WIN32)
    set(IRRLICHT_PATHS
      /usr/include
      /usr/include/irrlicht
      /usr/local/include
      /usr/local/include/irrlicht
      /opt/local/include
      /sw/include
    )

    set(IRRLICHT_LIBRARY_PATHS
      /usr/lib
      /usr/local/lib
      /opt/local/lib
      /sw/lib
    )
  endif (WIN32)

  # find Irrlicht
  find_path(IRRLICHT_INCLUDE_DIR
    NAMES
      irrlicht.h
    PATHS
      ${IRRLICHT_PATHS}
  )

  find_library(IRRLICHT_LIBRARY
    NAMES
        Irrlicht
    PATHS
      ${IRRLICHT_LIBRARY_PATHS}
  )

  if (IRRLICHT_LIBRARY)
    set(IRRLICHT_FOUND TRUE)
  endif (IRRLICHT_LIBRARY)

  set(IRRLICHT_INCLUDE_DIRS
    ${IRRLICHT_INCLUDE_DIR}
  )

  if (IRRLICHT_FOUND)
    set(IRRLICHT_LIBRARIES
      ${IRRLICHT_LIBRARIES}
      ${IRRLICHT_LIBRARY}
    )
  endif (IRRLICHT_FOUND)

  if (IRRLICHT_INCLUDE_DIRS AND IRRLICHT_LIBRARIES)
     set(IRRLICHT_FOUND TRUE)
  endif (IRRLICHT_INCLUDE_DIRS AND IRRLICHT_LIBRARIES)

  if (IRRLICHT_FOUND)
    if (NOT IRRLICHT_FIND_QUIETLY)
      message(STATUS "Found Irrlicht: ${IRRLICHT_LIBRARIES}")
    endif (NOT IRRLICHT_FIND_QUIETLY)
  else (IRRLICHT_FOUND)
    if (IRRLICHT_FIND_REQUIRED)
      message(FATAL_ERROR "Could not find Irrlicht")
    endif (IRRLICHT_FIND_REQUIRED)
  endif (IRRLICHT_FOUND)

  # show the IRRLICHT_INCLUDE_DIRS and IRRLICHT_LIBRARIES variables only in the advanced view
  mark_as_advanced(IRRLICHT_INCLUDE_DIRS IRRLICHT_LIBRARIES)

endif (IRRLICHT_LIBRARIES AND IRRLICHT_INCLUDE_DIRS)
