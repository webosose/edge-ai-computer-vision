cmake_minimum_required(VERSION 2.8.7)
project (edgeai-vision)

include(${CMAKE_SOURCE_DIR}/CMakeOptions.cmake)

set(CMAKE_CXX_STANDARD 17)  # for C++17
set(SRC_ROOT_DIR ${CMAKE_SOURCE_DIR})
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/third_party)

IF("${AIF_INSTALL_DIR}" STREQUAL "")
    set(AIF_INSTALL_DIR "/usr/share/aif")
ENDIF()

IF(NOT "${AIF_EXTENSION_REGISTRY_PATH}" STREQUAL "")
    ADD_DEFINITIONS(-DEDGEAI_VISION_EXTENSION_REGISTRY_PATH="${AIF_EXTENSION_REGISTRY_PATH}")
ENDIF()

IF(WITH_UPDATABLE_MODELS)
    ADD_DEFINITIONS(-DUSE_UPDATABLE_MODELS)
ENDIF(WITH_UPDATABLE_MODELS)

IF(WITH_NNAPI)
    ADD_DEFINITIONS(-DUSE_NNAPI)
ENDIF()

add_subdirectory(core)
add_subdirectory(extensions)
add_subdirectory(bin)
add_subdirectory(example)

IF(WITH_HOST_TEST)        ### for meta-shift unit test ###
    enable_testing()
    add_subdirectory(test)
ELSEIF(WITH_UNIT_TEST)
    add_subdirectory(test)
    add_subdirectory(test/test-dl)
    add_subdirectory(test/test-plugin)
    add_subdirectory(test/test-inspector)
    add_subdirectory(test-extra)
ENDIF()

add_subdirectory(third_party)
