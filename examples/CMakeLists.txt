cmake_minimum_required(VERSION 3.5)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_MODULE_PATH})

find_package(World REQUIRED QUIET)

if (${WORLD_FOUND})
    message(STATUS "World found at : ${WORLD_LIBRARIES}")

    message(STATUS "World include directory is : ${WORLD_INCLUDE_DIRS}")
    include_directories(${WORLD_INCLUDE_DIRS})

    add_executable(01_first_steps "01 - first steps/main.cpp")
    target_link_libraries(01_first_steps ${WORLD_LIBRARIES})
else()
    message(STATUS "World not found")
endif()
