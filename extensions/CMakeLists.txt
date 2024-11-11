cmake_minimum_required(VERSION 2.8.7)
project(edgeai-vision-base CXX)

set(EDGE_AI_VISION_VERSION_MAJOR 1)
set(EDGE_AI_VISION_VERSION_MINOR 0)
set(EDGE_AI_VISION_VERSION_PATCH 0)
set(EDGE_AI_VISION_VERSION_LIST
    "${EDGE_AI_VISION_VERSION_MAJOR}."
    "${EDGE_AI_VISION_VERSION_MINOR}."
    "${EDGE_AI_VISION_VERSION_PATCH}")
string(REPLACE ";" "" EDGE_AI_VISION_VERSION ${EDGE_AI_VISION_VERSION_LIST})

IF("${CMAKE_INSTALL_INCLUDEDIR}" STREQUAL "")
    set(CMAKE_INSTALL_INCLUDEDIR "/usr/include")
ENDIF()

include(FindPkgConfig)
find_package(Boost REQUIRED COMPONENTS system thread filesystem date_time iostreams)
find_package(xtensor REQUIRED)
find_package(xtl REQUIRED)
find_package(nlohmann_json REQUIRED)

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
pkg_check_modules(NLOHMANN_JSON REQUIRED nlohmann_json)

# include directories
include_directories(
    ${TFLITE_INCLUDE_DIRS}
    ${OPENCV_INCLUDE_DIRS}
    ${RAPIDJSON_INCLUDE_DIRS}
    ${PMLOGLIB_INCLUDE_DIRS}
    ${NLOHMANN_JSON_INCLUDE_DIRS}
)

# link directories
link_directories(
    ${TFLITE_LIBRARY_DIRS}
    ${OPENCV_LIBRARY_DIRS}
    ${RAPIDJSON_LIBRARY_DIRS}
    ${PMLOGLIB_LIBRARY_DIRS}
)

add_definitions(
    -DENABLE_DEBUG
    -DLOG_APPLICATION="AIF"
    -DEDGEAI_VISION_HOME="${AIF_INSTALL_DIR}"
    -DGAUSSIANDARK
    -DAFFINE_TRANS
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

IF(WITH_AUTO_DELEGATE)
    FIND_PACKAGE(PkgConfig REQUIRED)
    PKG_CHECK_MODULES(AUTO-DELEGATE REQUIRED auto-delegation)
    INCLUDE_DIRECTORIES(${AUTO-DELEGATE_INCLUDE_DIRS})
    LINK_DIRECTORIES(${AUTO-DELEGATE_LIBRARY_DIRS})
    ADD_DEFINITIONS(-DUSE_AUTO_DELEGATE)
ENDIF(WITH_AUTO_DELEGATE)

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -Wl,--no-as-needed -O2")

set(HDR_DIR ${CMAKE_SOURCE_DIR}/extensions)

# Header Files
include_directories(
    ${INCLUDE_DIR}
    ${HDR_DIR}
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
    xtensor
    xtensor::use_xsimd
    edgeai-vision
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

IF(WITH_AUTO_DELEGATE)
    set(LIBS
        ${LIBS}
        ${AUTO-DELEGATE_LIBRARIES}
    )
ENDIF(WITH_AUTO_DELEGATE)

IF(WITH_ARMNN)
    set(LIBS
        ${LIBS}
        ${ARMNN-DELEGATE_LIBRARIES}
    )
ENDIF(WITH_ARMNN)

# Base extension library
set(EXTENSION_NAME "${PROJECT_NAME}")
add_library(${EXTENSION_NAME}
    SHARED
    ${SRC_FILES}
)
set_target_properties(${EXTENSION_NAME} PROPERTIES
    VERSION ${EDGE_AI_VISION_VERSION}
    SOVERSION ${EDGE_AI_VISION_VERSION_MAJOR})
target_link_libraries(${EXTENSION_NAME}
    PRIVATE
    ${LIBS}
)

# Sub directories
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/bodypix")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/face")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/handLandmark")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/movenet")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/palm")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/pipe")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/pose")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/poseLandmark")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/selfie")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/semantic")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/signLanguageArea")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/text")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/plugin")

# Install
install(FILES ${CMAKE_SOURCE_DIR}/files/edgeai-vision-base.pc.in
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig RENAME edgeai-vision-base.pc)
install(FILES ${CMAKE_SOURCE_DIR}/files/edgeai-vision-all.pc.in
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig RENAME edgeai-vision-all.pc)
install(DIRECTORY
        ${CMAKE_SOURCE_DIR}/include/aifex
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(TARGETS ${EXTENSION_NAME}
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/edgeai-extensions)

IF(WITH_UPDATABLE_MODELS)
    install(FILES ${CMAKE_SOURCE_DIR}/model/yunet_yunet_final_360_640_simplify_float32.tflite DESTINATION ${AIF_INSTALL_DIR}/model)
ELSE()
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/model DESTINATION ${AIF_INSTALL_DIR})
ENDIF(WITH_UPDATABLE_MODELS)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/extra_models DESTINATION ${AIF_INSTALL_DIR})

