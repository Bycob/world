cmake_minimum_required(VERSION 2.8.6)


# Includes

include(CMakeInclude.cmake)

# Paramètres du projet

project(World)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
   message("Compiler : GNU")
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
   
   if(${CMAKE_BUILD_TYPE} STREQUAL Release)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -s -Os -Wno-attributes")
   endif()
elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
   message("Compiler : MSVC")
endif()


# Dépendances

include_directories(includes/
			modules/)


#Ajout de World API

add_subdirectory(modules/worldapi/)


#Executables de test

add_executable(test_terrain tests/test_terrain.cpp)
target_link_libraries(test_terrain worldapi)

add_executable(test_tree tests/test_tree.cpp)
target_link_libraries(test_tree worldapi)

add_executable(test_reliefmap tests/test_reliefmap.cpp)
target_link_libraries(test_reliefmap worldapi)
