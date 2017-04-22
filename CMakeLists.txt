# Version minimum de cmake
cmake_minimum_required(VERSION 2.8.6)

# Declaration du projet
project(World)

# Includes
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
include(cmake/WorldUtils.cmake)

# Options de l'utilisateur
option(WORLD_STATIC_BUILD "Build le projet sous formes de librairies statiques" OFF)
option(WORLD_BUILD_TESTS "Build les target de tests" ON)
option(WORLD_BUILD_WORLD3D "Build la visualisation en 3D" OFF)


# Setup de certaines variables
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

# Paramètres du projet
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Parametre du compilateur
if(CMAKE_CXX_COMPILER_ID MATCHES "^GNU$")
    message("-- Compiler : GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -pthread")

    if(CMAKE_BUILD_TYPE MATCHES "^Release$")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -s -Os -Wno-attributes")
    endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "^MSVC$")
    message("-- Compiler : MSVC")
endif()


# Dépendances
include(cmake/WorldDependencies.cmake)

include_directories(includes/
			modules/)


# Ajout de World API
add_subdirectory(modules/worldapi/)
# Ajout de World 3D
add_subdirectory(modules/world3D)
# Ajout du répertoire de tests
add_subdirectory(tests/)

message("=====")
