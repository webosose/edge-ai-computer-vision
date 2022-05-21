cmake_minimum_required(VERSION 2.8.7)
project(ai-framework CXX)

OPTION(WITH_EDGETPU "Enable Google Coral EdgeTPU Support" OFF)
OPTION(WITH_ARMNN "Enable ArmNN Delegate Support" ON)
OPTION(WITH_XNNPACK "Enable Xnnpack Delegate Support" ON)
OPTION(WITH_GPU "Enable Gpu Delegate Support" ON)

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
    -DLOG_CONTEXT="FACE"
    -DUSE_BASE64IMAGE
    -DEDGEAI_VISION_HOME="/usr/share/aif"
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

add_library(${LIB_NAME}
    SHARED
    ${SRC_FILES}
)

target_link_libraries(${LIB_NAME}
    ${LIBS}
)

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
