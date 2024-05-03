cmake_minimum_required(VERSION 2.8.7)
project(edgeai-vision-test-inspector CXX)

IF("${AIF_INSTALL_TEST_DIR}" STREQUAL "")
    set(AIF_INSTALL_TEST_DIR "${AIF_INSTALL_DIR}/test")
ENDIF()

include(FindPkgConfig)
find_package(Boost REQUIRED COMPONENTS system thread filesystem date_time iostreams)
find_package(GTest REQUIRED)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIRS})
endif()

# find needed packages
find_package(PkgConfig)
pkg_check_modules(GTEST REQUIRED gtest_main)
pkg_check_modules(TFLITE REQUIRED tensorflowlite)
pkg_check_modules(OPENCV REQUIRED opencv4)
pkg_check_modules(RAPIDJSON REQUIRED RapidJSON)
pkg_check_modules(PMLOGLIB REQUIRED PmLogLib)

# include directories
include_directories(
    ${GTEST_INCLUDE_DIRS}
    ${TFLITE_INCLUDE_DIRS}
    ${OPENCV_INCLUDE_DIRS}
    ${RAPIDJSON_INCLUDE_DIRS}
    ${PMLOGLIB_INCLUDE_DIRS}
)

# link directories
link_directories(
    ${GTEST_LIBRARY_DIRS}
    ${TFLITE_LIBRARY_DIRS}
    ${OPENCV_LIBRARY_DIRS}
    ${RAPIDJSON_LIBRARY_DIRS}
    ${PMLOGLIB_LIBRARY_DIRS}
)

add_definitions(
    -std=c++14
    -DENABLE_DEBUG
    -DLOG_APPLICATION="AIF"
    -DEDGEAI_VISION_HOME="${AIF_INSTALL_DIR}"
)

set(EXE_NAME ${PROJECT_NAME})
set(HDR_DIR ${CMAKE_SOURCE_DIR}/test/test-inspector)
set(SRC_DIR ${CMAKE_SOURCE_DIR}/test/test-inspector)

# Header Files
include_directories(
    ${INCLUDE_DIR}
    ${HDR_DIR}
    ${SRC_DIR}
    ${SRC_ROOT_DIR}/bin/inspector
)

link_directories(
)

# Source Files
set(SRC_FILES
    ${SRC_ROOT_DIR}/bin/inspector/ExtensionInspector.cpp
    ${SRC_DIR}/extensioninspector_test.cpp
)

set(LIBS
    ${Boost_LIBRARIES}
    ${TFLITE_LIBRARIES}
    ${OPENCV_LIBRARIES}
    ${RAPIDJSON_LIBRARIES}
    ${PMLOGLIB_LDFLAGS}
    pthread
    dl
    edgeai-vision
)

add_executable(
    ${EXE_NAME}
    ${SRC_FILES}
)

target_link_libraries(${EXE_NAME}
    ${GTEST_LIBRARIES}
    ${GTEST_MAIN_LIBRARIES}
    ${LIBS}
)

install(TARGETS ${EXE_NAME} DESTINATION ${AIF_INSTALL_TEST_DIR})
