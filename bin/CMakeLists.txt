cmake_minimum_required(VERSION 2.8.7)
project (edgeai-vision-bin)

IF("${AIF_INSTALL_BIN_DIR}" STREQUAL "")
    set(AIF_INSTALL_BIN_DIR "${AIF_INSTALL_DIR}/bin")
ENDIF()

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/inspector)
