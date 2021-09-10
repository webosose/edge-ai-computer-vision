cmake_minimum_required(VERSION 2.8.7)
project(ai-framework CXX)

OPTION(WITH_EDGETPU "Enable Google Coral EdgeTPU Support" OFF)

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
    -DUSE_BASE64IMAGE
)

IF(WITH_EDGETPU)
    FIND_PACKAGE(PkgConfig REQUIRED)
    PKG_CHECK_MODULES(EDGETPU REQUIRED edgetpu)
    INCLUDE_DIRECTORIES(${EDGETPU_INCLUDE_DIRS})
    LINK_DIRECTORIES(${EDGETPU_LIBRARY_DIRS})
    ADD_DEFINITIONS(-DUSE_EDGETPU)
ENDIF(WITH_EDGETPU)

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -Wl,--no-as-needed")

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