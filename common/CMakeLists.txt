file(GLOB SRCS *.cpp *.c)
file(GLOB FMT_SRCS fmt/fmt/*.cc)

add_library(common SHARED ${SRCS} ${FMT_SRCS})

