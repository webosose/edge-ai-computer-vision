cmake_minimum_required(VERSION 2.8.7)
project(edgeai-vision-test CXX)

OPTION(WITH_EDGETPU "Enable Google Coral EdgeTPU Support" OFF)
OPTION(WITH_ARMNN "Enable ArmNN Delegate Support" OFF)
OPTION(WITH_AUTO_DELEGATE "Enable Auto Delegate Selector Support" OFF)
OPTION(WITH_WEBSOKCET_TEST "Enable Auto Delegate Selector Support" OFF)

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

IF(WITH_EDGETPU)
    FIND_PACKAGE(PkgConfig REQUIRED)
    PKG_CHECK_MODULES(EDGETPU REQUIRED edgetpu)
    INCLUDE_DIRECTORIES(${EDGETPU_INCLUDE_DIRS})
    LINK_DIRECTORIES(${EDGETPU_LIBRARY_DIRS})
    ADD_DEFINITIONS(-DUSE_EDGETPU)
ENDIF(WITH_EDGETPU)

IF(WITH_ARMNN)
    FIND_PACKAGE(PkgConfig REQUIRED)
    PKG_CHECK_MODULES(ARMNN-DELEGATE REQUIRED armnn-delegate)
    INCLUDE_DIRECTORIES(${ARMNN-DELEGATE_INCLUDE_DIRS})
    LINK_DIRECTORIES(${ARMNN-DELEGATE_LIBRARY_DIRS})
    ADD_DEFINITIONS(-DUSE_ARMNN)
ENDIF(WITH_ARMNN)

IF(WITH_GPU)
    ADD_DEFINITIONS(-DUSE_GPU_DELEGATE)
ENDIF(WITH_GPU)

IF(WITH_AUTO_DELEGATE)
    FIND_PACKAGE(PkgConfig REQUIRED)
    PKG_CHECK_MODULES(AUTO-DELEGATE REQUIRED auto-delegation)
    INCLUDE_DIRECTORIES(${AUTO-DELEGATE_INCLUDE_DIRS})
    LINK_DIRECTORIES(${AUTO-DELEGATE_LIBRARY_DIRS})
    ADD_DEFINITIONS(-DUSE_AUTO_DELEGATE)
ENDIF(WITH_AUTO_DELEGATE)

set(EXE_NAME edgeai-vision-test)
set(HDR_DIR ${CMAKE_SOURCE_DIR}/test)
set(SRC_DIR ${CMAKE_SOURCE_DIR}/test)

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
    ${SRC_DIR}/api_facade_test.cpp
    ${SRC_DIR}/api_facade_multi_requests_test.cpp
    ${SRC_DIR}/capi_facade_test.cpp
    ${SRC_DIR}/detector_test.cpp
    ${SRC_DIR}/descriptor_test.cpp
    ${SRC_DIR}/detector_param_test.cpp
    ${SRC_DIR}/delegate_factory_test.cpp
    ${SRC_DIR}/detector_factory_test.cpp
    ${SRC_DIR}/logger_test.cpp
    ${SRC_DIR}/stopwatch_test.cpp
    ${SRC_DIR}/util_test.cpp
    ${SRC_DIR}/yunet_face_param_test.cpp
    ${SRC_DIR}/short_range_face_detector_test.cpp
    ${SRC_DIR}/face_descriptor_test.cpp
    ${SRC_DIR}/face_param_test.cpp
    ${SRC_DIR}/posenet_descriptor_test.cpp
    ${SRC_DIR}/posenet_detector_test.cpp
    ${SRC_DIR}/posenet_param_test.cpp
    ${SRC_DIR}/selfie_descriptor_test.cpp
    ${SRC_DIR}/selfie_detector_test.cpp
    ${SRC_DIR}/selfie_param_test.cpp
    ${SRC_DIR}/ai_vision_test.cpp
    ${SRC_DIR}/config_reader_test.cpp
    ${SRC_DIR}/performance_reporter_test.cpp
    ${SRC_DIR}/text_param_test.cpp
    ${SRC_DIR}/text_descriptor_test.cpp
    ${SRC_DIR}/text_detector_test.cpp
)

IF(WITH_EXTRA_MODELS)
    set(SRC_FILES
        ${SRC_FILES}
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
        ${SRC_DIR}/facemesh_descriptor_test.cpp
        ${SRC_DIR}/facemesh_detector_test.cpp
        ${SRC_DIR}/facemesh_param_test.cpp
    )
ENDIF(WITH_EXTRA_MODELS)

set(LIBS
    ${Boost_LIBRARIES}
    ${TFLITE_LIBRARIES}
    ${OPENCV_LIBRARIES}
    ${RAPIDJSON_LIBRARIES}
    ${PMLOGLIB_LDFLAGS}
    edgeai-vision
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

IF(WITH_ARMNN)
    set(SRC_FILES
        ${SRC_FILES}
        ${SRC_DIR}/armnn_delegate_test.cpp
    )
    set(LIBS
        ${LIBS}
        ${ARMNN-DELEGATE_LIBRARIES}
    )
ENDIF(WITH_ARMNN)

IF(WITH_GPU)
    set(SRC_FILES
        ${SRC_FILES}
        ${SRC_DIR}/gpu_delegate_test.cpp
    )
ENDIF(WITH_GPU)

IF(WITH_NPU)
    set(SRC_FILES
        ${SRC_FILES}
        ${SRC_DIR}/npu_delegate_test.cpp
    )
ENDIF(WITH_NPU)

IF(WITH_AUTO_DELEGATE)
    set(SRC_FILES
        ${SRC_FILES}
        ${SRC_DIR}/auto_delegate_test.cpp
    )
    set(LIBS
        ${LIBS}
        ${AUTO-DELEGATE_LIBRARIES}
    )
ENDIF(WITH_AUTO_DELEGATE)

IF(WITH_WEBSOCKET_TEST)
    set(SRC_FILES
        ${SRC_FILES}
        ${SRC_DIR}/websocket_test.cpp
        ${SRC_DIR}/websocket_bigsize_data_test.cpp
    )
ENDIF(WITH_WEBSOCKET_TEST)

add_executable(
    ${EXE_NAME}
    ${SRC_FILES}
)

target_link_libraries(${EXE_NAME}
    ${GTEST_LIBRARIES}
    ${GTEST_MAIN_LIBRARIES}
    ${LIBS}
)

IF(WITH_HOST_TEST)
  file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/test/tmp)
  file(COPY ${CMAKE_SOURCE_DIR}/images DESTINATION ${CMAKE_BINARY_DIR}/test/tmp)
  file(COPY ${CMAKE_SOURCE_DIR}/model DESTINATION ${CMAKE_BINARY_DIR}/test/tmp)
  file(COPY ${CMAKE_SOURCE_DIR}/extra_models/ DESTINATION ${CMAKE_BINARY_DIR}/test/tmp/model/)

  ADD_DEFINITIONS(-DUSE_HOST_TEST)
  gtest_add_tests(${EXE_NAME} "" AUTO)
ENDIF(WITH_HOST_TEST)

add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/pipe")

IF(WITH_FITTV)
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/fittv")
ENDIF(WITH_FITTV)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/images DESTINATION ${AIF_INSTALL_DIR})
install(TARGETS ${EXE_NAME} DESTINATION ${AIF_INSTALL_TEST_DIR})
