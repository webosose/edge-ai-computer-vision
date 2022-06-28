cmake_minimum_required(VERSION 2.8.7)
project (edgeai-vision)

OPTION(WITH_UPDATABLE_MODELS "Enable Updatable Models" OFF)
OPTION(WITH_EXTRA_MODELS "Enable Extra Models" OFF)
OPTION(WITH_EXAMPLES "Enable Examples" OFF)
OPTION(WITH_UNIT_TEST "Enable UnitTests" ON)

set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

IF("${AIF_INSTALL_DIR}" STREQUAL "")
    set(AIF_INSTALL_DIR "/usr/share/aif")
ENDIF()

IF(WITH_UPDATABLE_MODELS)
    ADD_DEFINITIONS(-DUSE_UPDATABLE_MODELS)
ENDIF(WITH_UPDATABLE_MODELS)

IF(WITH_EXTRA_MODELS)
    ADD_DEFINITIONS(-DUSE_EXTRA_MODELS)
ENDIF(WITH_EXTRA_MODELS)

add_subdirectory(framework)

IF(WITH_EXAMPLES)
    add_subdirectory(example)
ENDIF()

IF(WITH_UNIT_TEST)
    add_subdirectory(test)
ENDIF()
