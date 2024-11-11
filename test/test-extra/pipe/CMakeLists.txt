cmake_minimum_required(VERSION 2.8.7)

set(EXTRA_PIPE_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(EXTRA_PIPE_SRC
    ${EXTRA_PIPE_SRC_DIR}/extra_pipe_api_test.cpp
    ${EXTRA_PIPE_SRC_DIR}/palm_crop_operation_test.cpp
    ${EXTRA_PIPE_SRC_DIR}/hand_gesture_pipe_test.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${EXTRA_PIPE_SRC_DIR})
target_sources(${PROJECT_NAME} PRIVATE ${EXTRA_PIPE_SRC})
