cmake_minimum_required(VERSION 3.2)
project(gltut)

include_directories(AFTER SYSTEM ${CMAKE_INSTALL_PREFIX}/include)
include_directories(common)
include_directories(common/fmt)

set(CMAKE_CXX_STANDARD 11)

add_subdirectory(common)
add_subdirectory(program)
add_subdirectory(scene)



