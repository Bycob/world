message(STATUS "Build ZLib")

file(GLOB ZLIB_SRCS *.c)

add_library(zlib STATIC ${ZLIB_SRCS})
set_property(TARGET zlib PROPERTY POSITION_INDEPENDENT_CODE ON)