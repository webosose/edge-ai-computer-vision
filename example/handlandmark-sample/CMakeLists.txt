cmake_minimum_required(VERSION 2.8.7)
project(handlandmark-sample CXX)

OPTION(WITH_EDGETPU "Enable Google Coral EdgeTPU Support" OFF)
OPTION(WITH_ARMNN "Enable ArmNN Delegate Support" OFF)
OPTION(WITH_XNNPACK "Enable XNNPack Delegate Support" ON)

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


IF(WITH_ARMNN)
PKG_CHECK_MODULES(ARMNN-DELEGATE REQUIRED armnn-delegate)
ENDIF(WITH_ARMNN)

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
    -DLOG_CONTEXT="POSELANDMARK"
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
    ${Boost_LIBRARIES}
    ${TFLITE_LIBRARIES}
    ${OPENCV_LIBRARIES}
    ${RAPIDJSON_LIBRARIES}
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
        ${ARMNN-DELEGATE_SRCS}
    )
set(LIBS
        ${LIBS}
        ${ARMNN-DELEGATE_LIBRARIES}
    )
ENDIF(WITH_ARMNN)

add_executable(
    ${PROJECT_NAME}
    ${SRC_FILES}
)

target_link_libraries(${PROJECT_NAME}
    ${LIBS}
)

# Install Artifacts
#install (
#    PROGRAMS ${CMAKE_BINARY_DIR}/${PROJECT_NAME}/${EXE_NAME}
#    DESTINATION ${CMAKE_INSTALL_BINDIR}
#)

