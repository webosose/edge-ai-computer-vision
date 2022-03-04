cmake_minimum_required(VERSION 2.8.7)
project(ai-framework-test CXX)

OPTION(WITH_EDGETPU "Enable Google Coral EdgeTPU Support" OFF)
OPTION(WITH_ARMNN "Enable ArmNN Delegate Support" ON)

#set(BOOST_ROOT /home/worker)
#set(BOOST_INCLUDEDIR $BOOST_ROOT/include/boost)

include(FindPkgConfig)
find_package(Boost REQUIRED COMPONENTS system thread filesystem date_time iostreams)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIRS})
endif()

# find needed packages
find_package(PkgConfig)
pkg_check_modules(TFLITE REQUIRED tensorflowlite)
pkg_check_modules(OPENCV REQUIRED opencv4)
pkg_check_modules(RAPIDJSON REQUIRED RapidJSON)

# include directories
include_directories(
    ${TFLITE_INCLUDE_DIRS}
    ${OPENCV_INCLUDE_DIRS}
    ${RAPIDJSON_INCLUDE_DIRS}
)

# link directories
link_directories(
    ${TFLITE_LIBRARY_DIRS}
    ${OPENCV_LIBRARY_DIRS}
    ${RAPIDJSON_LIBRARY_DIRS}
)

add_definitions(
    -std=c++14
    -DENABLE_DEBUG
    -DLOG_APPLICATION="AIF"
    -DLOG_CONTEXT="FACE"
)

IF(WITH_EDGETPU)
    FIND_PACKAGE(PkgConfig REQUIRED)
    PKG_CHECK_MODULES(EDGETPU REQUIRED edgetpu)
    INCLUDE_DIRECTORIES(${EDGETPU_INCLUDE_DIRS})
    LINK_DIRECTORIES(${EDGETPU_LIBRARY_DIRS})
    ADD_DEFINITIONS(-DUSE_EDGETPU)
ENDIF(WITH_EDGETPU)

IF(WITH_ARMNN)
    ADD_DEFINITIONS(-DUSE_ARMNN)
ENDIF(WITH_ARMNN)

set(EXE_NAME ai-framework-test)
set(HDR_DIR ${CMAKE_SOURCE_DIR}/test)
set(SRC_DIR ${CMAKE_SOURCE_DIR}/test)
set(GTEST_DIR ${CMAKE_SOURCE_DIR}/test/gtest)

# Header Files
include_directories(
    ${INCLUDE_DIR}
    ${HDR_DIR}
    ${SRC_DIR}
    ${GTEST_DIR}
)

link_directories(
)

# Source Files

set(SRC_FILES
    ${SRC_DIR}/detector_test.cpp
    ${SRC_DIR}/descriptor_test.cpp
    ${SRC_DIR}/detector_param_test.cpp
    ${SRC_DIR}/delegate_factory_test.cpp
    ${SRC_DIR}/detector_factory_test.cpp
    ${SRC_DIR}/logger_test.cpp
    ${SRC_DIR}/stopwatch_test.cpp
    ${SRC_DIR}/websocket_test.cpp
    ${SRC_DIR}/websocket_bigsize_data_test.cpp
    ${SRC_DIR}/util_test.cpp
    ${SRC_DIR}/face_descriptor_test.cpp
    ${SRC_DIR}/face_detector_test.cpp
    ${SRC_DIR}/face_param_test.cpp
    ${SRC_DIR}/posenet_descriptor_test.cpp
    ${SRC_DIR}/posenet_detector_test.cpp
    ${SRC_DIR}/movenet_descriptor_test.cpp
    ${SRC_DIR}/movenet_detector_test.cpp
    ${SRC_DIR}/bodypix_descriptor_test.cpp
    ${SRC_DIR}/bodypix_detector_test.cpp
    ${SRC_DIR}/selfie_descriptor_test.cpp
    ${SRC_DIR}/selfie_detector_test.cpp
    ${SRC_DIR}/semantic_descriptor_test.cpp
    ${SRC_DIR}/semantic_detector_test.cpp
    ${SRC_DIR}/poselandmark_descriptor_test.cpp
    ${SRC_DIR}/poselandmark_detector_test.cpp
    ${SRC_DIR}/handlandmark_descriptor_test.cpp
    ${SRC_DIR}/handlandmark_detector_test.cpp
    ${SRC_DIR}/palm_descriptor_test.cpp
    ${SRC_DIR}/palm_detector_test.cpp
    ${SRC_DIR}/palm_param_test.cpp
    ${SRC_DIR}/armnn_delegate_test.cpp
    ${GTEST_DIR}/gtest-all.cc
    ${GTEST_DIR}/test_main.cpp
)

set(LIBS
    ${Boost_LIBRARIES}
    ${TFLITE_LIBRARIES}
    ${OPENCV_LIBRARIES}
    ${RAPIDJSON_LIBRARIES}
    ai-framework
    pthread
)

IF(WITH_EDGETPU)
    set(EDGETPU_SRCS
    )
    set(SRC_FILES
        ${SRC_FILES}
        ${EDGETPU_SRCS}
    )
    set(LIBS
        ${LIBS}
        ${EDGETPU_LIBRARIES}
    )
ENDIF(WITH_EDGETPU)

add_executable(
    ${EXE_NAME}
    ${SRC_FILES}
)

target_link_libraries(${EXE_NAME}
    ${LIBS}
)

# Install Artifacts
#install (
#    PROGRAMS ${CMAKE_BINARY_DIR}/${PROJECT_NAME}/${EXE_NAME}
#    DESTINATION ${CMAKE_INSTALL_BINDIR}
#)

