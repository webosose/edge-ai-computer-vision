cmake_minimum_required(VERSION 2.8.7)

set(PIPE_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(PIPE_SRC
    ${PIPE_SRC_DIR}/CropOperation.cpp
    ${PIPE_SRC_DIR}/PalmCropOperation.cpp
)

target_include_directories(${EXTENSION_NAME} PRIVATE ${PIPE_SRC_DIR})
target_sources(${EXTENSION_NAME} PRIVATE ${PIPE_SRC})
