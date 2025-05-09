cmake_minimum_required(VERSION 2.8.7)
project(edgeai-vision-test CXX)

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

IF(WITH_NPU)
    FIND_PACKAGE(PkgConfig REQUIRED)
    PKG_CHECK_MODULES(NPU-DELEGATE REQUIRED npu-delegate)
    INCLUDE_DIRECTORIES(${NPU-DELEGATE_INCLUDE_DIRS})
    LINK_DIRECTORIES(${NPU-DELEGATE_LIBRARY_DIRS})
    ADD_DEFINITIONS(-DUSE_NPU)
ENDIF(WITH_NPU)

IF(WITH_AUTO_DELEGATE)
    FIND_PACKAGE(PkgConfig REQUIRED)
    PKG_CHECK_MODULES(AUTO-DELEGATE REQUIRED auto-delegation)
    INCLUDE_DIRECTORIES(${AUTO-DELEGATE_INCLUDE_DIRS})
    LINK_DIRECTORIES(${AUTO-DELEGATE_LIBRARY_DIRS})
    ADD_DEFINITIONS(-DUSE_AUTO_DELEGATE)
ENDIF(WITH_AUTO_DELEGATE)

set(EXE_NAME ${PROJECT_NAME})
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
    ${SRC_DIR}/face_descriptor_test.cpp
    ${SRC_DIR}/face_param_test.cpp
    ${SRC_DIR}/face_aifparam_test.cpp
    ${SRC_DIR}/ai_vision_test.cpp
    ${SRC_DIR}/config_reader_test.cpp
    ${SRC_DIR}/performance_reporter_test.cpp
    ${SRC_DIR}/extra_output_test.cpp
    ${SRC_DIR}/extensionloader_test.cpp
)

IF(WITH_UPDATABLE_MODELS)
    set(SRC_FILES
        ${SRC_FILES}
    )
ELSE()
    set(SRC_FILES
        ${SRC_FILES}
        ${SRC_DIR}/posenet_descriptor_test.cpp
        ${SRC_DIR}/posenet_detector_test.cpp
        ${SRC_DIR}/posenet_param_test.cpp
        ${SRC_DIR}/selfie_descriptor_test.cpp
        ${SRC_DIR}/selfie_detector_test.cpp
        ${SRC_DIR}/selfie_param_test.cpp
        ${SRC_DIR}/text_param_test.cpp
        ${SRC_DIR}/text_descriptor_test.cpp
        ${SRC_DIR}/text_detector_test.cpp
        ${SRC_DIR}/text_api_test.cpp
        ${SRC_DIR}/signlanguage_descriptor_test.cpp
        ${SRC_DIR}/signlanguage_detector_test.cpp
        ${SRC_DIR}/signlanguage_api_test.cpp
    )
ENDIF(WITH_UPDATABLE_MODELS)

set(LIBS
    ${TFLITE_LIBRARIES}
    ${Boost_LIBRARIES}
    ${OPENCV_LIBRARIES}
    ${RAPIDJSON_LIBRARIES}
    ${PMLOGLIB_LDFLAGS}
    edgeai-vision
    edgeai-vision-base
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
    set(LIBS
        ${LIBS}
        ${NPU-DELEGATE_LIBRARIES}
    )
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

#If you want to link edgeai-extensions library(i.e. edgeai-vision-base), you can use the following to set the rpath.
set(R_PATH "/usr/lib/edgeai-extensions")
set_target_properties(${EXE_NAME}
        PROPERTIES
        LINK_FLAGS "-Wl,-rpath,${R_PATH}"
)

IF(WITH_HOST_TEST)
  file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/test/tmp)
  file(COPY ${CMAKE_SOURCE_DIR}/images/* DESTINATION ${CMAKE_BINARY_DIR}/test/tmp/images)
  file(COPY ${CMAKE_SOURCE_DIR}/model DESTINATION ${CMAKE_BINARY_DIR}/test/tmp)
  file(COPY ${CMAKE_SOURCE_DIR}/extra_models/ DESTINATION ${CMAKE_BINARY_DIR}/test/tmp/model/)

  ADD_DEFINITIONS(-DUSE_HOST_TEST)
  gtest_add_tests(${EXE_NAME} "" AUTO)
ENDIF(WITH_HOST_TEST)

add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/pipe")

file(GLOB TEST_IMAGES "${CMAKE_SOURCE_DIR}/images/*")
install(FILES ${TEST_IMAGES} DESTINATION ${AIF_INSTALL_DIR}/images)
install(TARGETS ${EXE_NAME} DESTINATION ${AIF_INSTALL_TEST_DIR})
