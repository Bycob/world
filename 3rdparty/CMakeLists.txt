if(NOT ${WORLD_USE_OPENCV})
    add_subdirectory(zlib)
    add_subdirectory(libpng)

    set(THIRD_PARTY_LIBRARIES zlib libpng)
    set(THIRD_PARTY_INCLUDES ${PROJECT_SOURCE_DIR}/3rdparty/zlib/)
else()
    set(THIRD_PARTY_LIBRARIES "")
endif()