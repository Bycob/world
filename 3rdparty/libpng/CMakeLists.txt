message("-- Build Libpng")

file(GLOB LIBPNG_SRCS *.c)

add_library(libpng STATIC ${LIBPNG_SRCS})
target_link_libraries(libpng zlib)
target_include_directories(libpng PUBLIC "${PROJECT_SOURCE_DIR}/3rdparty/zlib/")