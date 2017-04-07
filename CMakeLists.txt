cmake_minimum_required(VERSION 2.8.6)


# Includes

include (GenerateExportHeader)

# Fonction tirée de : http://stackoverflow.com/questions/31422680/how-to-set-visual-studio-filters-for-nested-sub-directory-using-cmake
function(assign_source_group)
    foreach(_source IN ITEMS ${ARGN})
        if (IS_ABSOLUTE "${_source}")
            file(RELATIVE_PATH _source_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${_source}")
        else()
            set(_source_rel "${_source}")
        endif()

        get_filename_component(_source_path "${_source_rel}" PATH)
        string(REPLACE "/" "\\" _source_path_msvc "${_source_path}")
        source_group("${_source_path_msvc}" FILES "${_source}")
    endforeach()
endfunction(assign_source_group)


# Paramètres du projet

project(World)

set(EXECUTABLE_OUTPUT_PATH bin)
set(LIBRARY_OUTPUT_PATH bin)

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

find_package(OpenCV REQUIRED)


#Declaration de World API

file(GLOB_RECURSE
	WORLD_API_SOURCES
	modules/worldapi/*)

add_library(worldapi SHARED ${WORLD_API_SOURCES})
target_link_libraries(worldapi ${OpenCV_LIBS})

assign_source_group(${WORLD_API_SOURCES})


#Executables de test

link_directories(${CMAKE_BINARY_DIR})

add_executable(test_terrain tests/test_terrain.cpp)
target_link_libraries(test_terrain worldapi)

add_executable(test_tree tests/test_tree.cpp)
target_link_libraries(test_tree worldapi)

add_executable(test_reliefmap tests/test_reliefmap.cpp)
target_link_libraries(test_reliefmap worldapi)
