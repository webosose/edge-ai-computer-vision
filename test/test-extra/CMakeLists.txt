cmake_minimum_required(VERSION 2.8.7)
project(edgeai-vision-test-extra CXX)

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
    -DENABLE_DEBUG
    -DLOG_APPLICATION="AIF"
    -DEDGEAI_VISION_HOME="${AIF_INSTALL_DIR}"
)

set(EXE_NAME ${PROJECT_NAME})
set(TEST_HDR_DIR ${CMAKE_SOURCE_DIR}/test)
set(HDR_DIR ${CMAKE_SOURCE_DIR}/test/test-extra)
set(SRC_DIR ${CMAKE_SOURCE_DIR}/test/test-extra)

# Header Files
include_directories(
    ${INCLUDE_DIR}
    ${TEST_HDR_DIR}
    ${HDR_DIR}
    ${SRC_DIR}
)

link_directories(
)

# Source Files
set(SRC_FILES
    ${SRC_FILES}
    ${SRC_DIR}/short_range_face_detector_test.cpp
    ${SRC_DIR}/full_range_face_detector_test.cpp
    ${SRC_DIR}/movenet_descriptor_test.cpp
    ${SRC_DIR}/movenet_detector_test.cpp
    ${SRC_DIR}/movenet_param_test.cpp
    ${SRC_DIR}/bodypix_descriptor_test.cpp
    ${SRC_DIR}/bodypix_detector_test.cpp
    ${SRC_DIR}/bodypix_param_test.cpp
    ${SRC_DIR}/semantic_descriptor_test.cpp
    ${SRC_DIR}/semantic_detector_test.cpp
    ${SRC_DIR}/semantic_param_test.cpp
    ${SRC_DIR}/poselandmark_descriptor_test.cpp
    ${SRC_DIR}/poselandmark_detector_test.cpp
    ${SRC_DIR}/poselandmark_param_test.cpp
    ${SRC_DIR}/handlandmark_descriptor_test.cpp
    ${SRC_DIR}/handlandmark_detector_test.cpp
    ${SRC_DIR}/handlandmark_param_test.cpp
    ${SRC_DIR}/palm_descriptor_test.cpp
    ${SRC_DIR}/palm_detector_test.cpp
    ${SRC_DIR}/palm_param_test.cpp
    ${SRC_DIR}/yunet_v2_face_detector_test.cpp
    ${SRC_DIR}/yunet_face_detector_test.cpp
)

set(LIBS
    ${Boost_LIBRARIES}
    ${TFLITE_LIBRARIES}
    ${OPENCV_LIBRARIES}
    ${RAPIDJSON_LIBRARIES}
    ${PMLOGLIB_LDFLAGS}
    pthread
    edgeai-vision
    edgeai-vision-base
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
#If you want to link edgeai-extensions library(i.e. edgeai-vision-base), you can use the following to set the rpath.
set(R_PATH "/usr/lib/edgeai-extensions")
set_target_properties(${EXE_NAME}
        PROPERTIES
        LINK_FLAGS "-Wl,-rpath,${R_PATH}"
)

add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/pipe")
install(TARGETS ${EXE_NAME} DESTINATION ${AIF_INSTALL_TEST_DIR})
