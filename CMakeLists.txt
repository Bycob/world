# Version minimum de cmake
cmake_minimum_required(VERSION 2.8.6)

# Includes
include(CMakeInclude.cmake)

# Options de l'utilisateur
option(WORLD_STATIC_BUILD "Build le projet sous formes de librairies statiques" OFF)


# Setup de certaines variables
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

# Paramètres du projet
project(World)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Parametre du compilateur
if(${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
   message("-- Compiler : GNU")
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
   
   if(${CMAKE_BUILD_TYPE} STREQUAL Release)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -s -Os -Wno-attributes")
   endif()
elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
   message("-- Compiler : MSVC")
endif()


# Dépendances
include_directories(includes/
			modules/)


# Ajout de World API
add_subdirectory(modules/worldapi/)
# Ajout du répertoire de tests
add_subdirectory(tests/)
