message("-- Build ZLib")

file(GLOB ZLIB_SRCS *.c)

add_library(zlib STATIC ${ZLIB_SRCS})