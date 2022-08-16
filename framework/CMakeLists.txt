cmake_minimum_required(VERSION 2.8.7)
project(edgeai-vision CXX)

set(EDGE_AI_VISION_VERSION_MAJOR 1)
set(EDGE_AI_VISION_VERSION_MINOR 0)
set(EDGE_AI_VISION_VERSION_PATCH 0)
set(EDGE_AI_VISION_VERSION_LIST
    "${EDGE_AI_VISION_VERSION_MAJOR}."
    "${EDGE_AI_VISION_VERSION_MINOR}."
    "${EDGE_AI_VISION_VERSION_PATCH}")
string(REPLACE ";" "" EDGE_AI_VISION_VERSION ${EDGE_AI_VISION_VERSION_LIST})

OPTION(WITH_EDGETPU "Enable Google Coral EdgeTPU Support" OFF)
OPTION(WITH_ARMNN "Enable ArmNN Delegate Support" OFF)
OPTION(WITH_XNNPACK "Enable Xnnpack Delegate Support" ON)
OPTION(WITH_GPU "Enable Gpu Delegate Support" OFF)
OPTION(WITH_AUTO_DELEGATE "Enable Auto Delegate Selector Support" OFF)
OPTION(WITH_NPU "Enable Npu Delegate Support" ON)

IF("${CMAKE_INSTALL_INCLUDEDIR}" STREQUAL "")
    set(CMAKE_INSTALL_INCLUDEDIR "/usr/include")
ENDIF()

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
pkg_check_modules(PMLOGLIB REQUIRED PmLogLib)

# include directories
include_directories(
    ${TFLITE_INCLUDE_DIRS}
    ${OPENCV_INCLUDE_DIRS}
    ${RAPIDJSON_INCLUDE_DIRS}
    ${PMLOGLIB_INCLUDE_DIRS}
)

# link directories
link_directories(
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

IF(WITH_XNNPACK)
    ADD_DEFINITIONS(-DUSE_XNNPACK)
ENDIF(WITH_XNNPACK)

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

IF(WITH_UPDATABLE_MODELS)
    FIND_PACKAGE(PkgConfig REQUIRED)
    PKG_CHECK_MODULES(GLIB2 REQUIRED glib-2.0)
    INCLUDE_DIRECTORIES(${GLIB2_INCLUDE_DIRS})
    LINK_DIRECTORIES(${GLIB2_LIBRARY_DIRS})
ENDIF(WITH_UPDATABLE_MODELS)

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -Wl,--no-as-needed -O2")

set(LIB_NAME ${PROJECT_NAME})
set(HDR_DIR ${CMAKE_SOURCE_DIR}/framework)
set(SRC_DIR ${CMAKE_SOURCE_DIR}/framework)

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
)

set(LIBS
    ${Boost_LIBRARIES}
    ${TFLITE_LIBRARIES}
    ${OPENCV_LIBRARIES}
    ${RAPIDJSON_LIBRARIES}
    ${PMLOGLIB_LDFLAGS}
    pthread
)

IF(WITH_EDGETPU)
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
        ${ARMNN_DELEGATE_SRCS}
    )
    set(LIBS
        ${LIBS}
        ${ARMNN-DELEGATE_LIBRARIES}
    )
ENDIF(WITH_ARMNN)

IF(WITH_GPU)
    set(SRC_FILES
        ${SRC_FILES}
        ${GPU_DELEGATE_SRCS}
    )
ENDIF(WITH_GPU)

IF(WITH_NPU)
    set(SRC_FILES
        ${SRC_FILES}
        ${NPU_DELEGATE_SRCS}
    )
    set(LIBS
        ${LIBS}
        ${NPU-DELEGATE_LIBRARIES}
    )
ENDIF(WITH_NPU)

IF(WITH_AUTO_DELEGATE)
    set(LIBS
        ${LIBS}
        ${AUTO-DELEGATE_LIBRARIES}
    )
ENDIF(WITH_AUTO_DELEGATE)

IF(WITH_UPDATABLE_MODELS)
    set(LIBS
        ${LIBS}
        ailoader
    )
ENDIF(WITH_UPDATABLE_MODELS)

add_library(${LIB_NAME}
    SHARED
    ${SRC_FILES}
)
set_target_properties(${LIB_NAME} PROPERTIES
    VERSION ${EDGE_AI_VISION_VERSION}
    SOVERSION ${EDGE_AI_VISION_VERSION_MAJOR})
target_link_libraries(${LIB_NAME}
    ${LIBS}
)

install(FILES ${CMAKE_SOURCE_DIR}/files/edgeai-vision.pc.in
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig RENAME edgeai-vision.pc)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/aif
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(TARGETS edgeai-vision
    DESTINATION ${CMAKE_INSTALL_LIBDIR})

IF(WITH_UPDATABLE_MODELS)
    install(FILES ${CMAKE_SOURCE_DIR}/model/yunet_yunet_final_360_640_simplify_float32.tflite DESTINATION ${AIF_INSTALL_DIR}/model)
ELSE()
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/model DESTINATION ${AIF_INSTALL_DIR})
ENDIF(WITH_UPDATABLE_MODELS)

IF(WITH_EXTRA_MODELS)
    file(GLOB EXTRA_MODELS "${CMAKE_SOURCE_DIR}/extra_models/*.tflite")
    install(FILES ${EXTRA_MODELS} DESTINATION ${AIF_INSTALL_DIR}/model)
ENDIF(WITH_EXTRA_MODELS)

add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/log")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/tools")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/base")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/face")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/pose")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/movenet")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/semantic")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/bodypix")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/selfie")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/poseLandmark")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/handLandmark")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/palm")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/delegate")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/sample")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/facade")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/text")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/pipe")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/bodyPoseEstimation")
