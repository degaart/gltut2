file(GLOB SRCS *.cpp)
file(GLOB HDRS *.h)

add_library(scene SHARED ${SRCS})
target_link_libraries(scene common)



