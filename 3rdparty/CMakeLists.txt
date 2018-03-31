
set(THIRD_PARTY_LIBRARIES "")

if((NOT ${WORLD_BUILD_OPENCV_MODULES}) OR (NOT ${WORLD_HAS_OPENCV}))
    add_subdirectory(zlib)
    add_subdirectory(libpng)

    set(THIRD_PARTY_LIBRARIES ${THIRD_PARTY_LIBRARIES} zlib libpng)
    set(THIRD_PARTY_INCLUDES ${PROJECT_SOURCE_DIR}/3rdparty/zlib/)
endif()