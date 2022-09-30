cmake_minimum_required(VERSION 2.8.7)
project(dl-sample CXX)

find_package(PkgConfig)
pkg_check_modules(OPENCV REQUIRED opencv4)

include_directories(
    ${OPENCV_INCLUDE_DIRS}
)

link_directories(
    ${OPENCV_LIBRARY_DIRS}
)

add_definitions(
    -std=c++14
    -DENABLE_DEBUG
)

set(HDR_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# Header Files
include_directories(
    ${INCLUDE_DIR}
    ${HDR_DIR}
    ${SRC_DIR}
)

link_directories(
)

# Source Files
set(SRC_FILES
    ${SRC_DIR}/main.cpp
)

set(LIBS
    ${OPENCV_LIBRARIES}
    dl
    pthread
)

add_executable(
    ${PROJECT_NAME}
    ${SRC_FILES}
)

target_link_libraries(${PROJECT_NAME}
    ${LIBS}
)

install(TARGETS ${PROJECT_NAME} DESTINATION ${AIF_INSTALL_EXAMPLE_DIR})
