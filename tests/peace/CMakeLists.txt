
if (${WORLD_BUILD_TESTS} AND ${WORLD_BUILD_PEACE})

    set(PEACE_TESTS_SOURCES
            test_peace.cpp)

    add_executable(runtests_peace ${PEACE_TESTS_SOURCES})
    target_link_libraries(runtests_peace world)
endif()