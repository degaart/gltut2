file(GLOB SRCS *.cpp)
file(GLOB HDRS *.h)
file(GLOB RSRC ../res/*)

set_source_files_properties(${RSRC}
    PROPERTIES
    MACOSX_PACKAGE_LOCATION Resources)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -ObjC")

add_executable(${CMAKE_PROJECT_NAME} MACOSX_BUNDLE ${SRCS} ${HDRS} ${RSRC})
target_link_libraries(${CMAKE_PROJECT_NAME}
    common
    "-framework Cocoa"
    "-framework IOKit" 
    "-framework CoreFoundation" 
    "-framework CoreVideo"
	"-framework OpenGL"
    ${CMAKE_INSTALL_PREFIX}/lib/libglfw3.a)
set_property(TARGET ${CMAKE_PROJECT_NAME}
    PROPERTY MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in
)



