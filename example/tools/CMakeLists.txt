cmake_minimum_required(VERSION 2.8.7)
project(example-tools CXX)

include(FindPkgConfig)
find_package(Boost REQUIRED COMPONENTS system thread filesystem date_time iostreams)

find_package(PkgConfig)
pkg_check_modules(OPENCV REQUIRED opencv4)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIRS})
endif()

add_definitions(
    -DENABLE_DEBUG
    -DLOG_APPLICATION="AIF"
)

set(LIB_NAME "edgeai-${PROJECT_NAME}")
set(HDR_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# Header Files
include_directories(
    ${INCLUDE_DIR}
    ${HDR_DIR}
    ${SRC_DIR}
    ${OPENCV_INCLUDE_DIRS}
)

link_directories(
    ${OPENCV_LIBRARY_DIRS}
)

# Source Files

set(SRC_FILES
    ${SRC_DIR}/Bbox.cpp
    ${SRC_DIR}/WSClient.cpp
    ${SRC_DIR}/WSServer.cpp
    ${SRC_DIR}/WSServerSession.cpp
    ${SRC_DIR}/VideoTester.cpp
    ${SRC_DIR}/Renderer.cpp
    ${SRC_DIR}/Sample.cpp
)

set(LIBS
    ${Boost_LIBRARIES}
    ${OPENCV_LIBRARIES}
    edgeai-vision
    pthread
)

add_library(${LIB_NAME}
    SHARED
    ${SRC_FILES}
)

set_target_properties(${LIB_NAME} PROPERTIES
    VERSION 1.0.0
    SOVERSION 1
)

target_include_directories(${LIB_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/)
target_link_libraries(${LIB_NAME}
    ${LIBS}
)

file(GLOB EXAMPLE_TOOLS_HDRS
    "${CMAKE_CURRENT_LIST_DIR}/*.h"
)
install(FILES ${EXAMPLE_TOOLS_HDRS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/aif/example-tools)
install(TARGETS ${LIB_NAME} DESTINATION ${CMAKE_INSTALL_LIBDIR})
