
set(THIRD_PARTY_LIBRARIES "")
set(THIRD_PARTY_INCLUDES "")

if((NOT ${WORLD_BUILD_OPENCV_MODULES}) OR (NOT ${WORLD_HAS_OPENCV}))
    add_subdirectory(zlib)
    add_subdirectory(libpng)

    set(THIRD_PARTY_LIBRARIES ${THIRD_PARTY_LIBRARIES} zlib libpng)
endif()

set(THIRD_PARTY_LIBRARIES ${THIRD_PARTY_LIBRARIES} PARENT_SCOPE)
set(THIRD_PARTY_INCLUDES ${THIRD_PARTY_INCLUDES} PARENT_SCOPE)

install(FILES optional/optional.hpp DESTINATION include/optional)
install(DIRECTORY armadillo DESTINATION include)