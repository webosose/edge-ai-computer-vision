cmake_minimum_required(VERSION 2.8.7)
project (edgeai-vision)

OPTION(WITH_UPDATABLE_MODELS "Enable Updatable Models" OFF)
OPTION(WITH_EXTRA_MODELS "Enable Extra Models" ON)
OPTION(WITH_EXAMPLES "Enable Examples" OFF)
OPTION(WITH_UNIT_TEST "Enable UnitTests" ON)
OPTION(WITH_HOST_TEST "Enable Host Based Unit test" OFF)
OPTION(WITH_FITTV "Enable FitTV Model and Example" ON)

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

IF(WITH_HOST_TEST)        ### for meta-shift unit test ###
    enable_testing()
    add_subdirectory(test)
ELSEIF(WITH_UNIT_TEST)
    add_subdirectory(test)
    add_subdirectory(test/test-dl)
ENDIF()

IF(WITH_FITTV)
    ADD_DEFINITIONS(-DUSE_FITTV)
    add_subdirectory(example)
ENDIF()
