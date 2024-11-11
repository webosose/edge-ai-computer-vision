cmake_minimum_required(VERSION 2.8.7)

set(PIPE_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(PIPE_SRC
    ${PIPE_SRC_DIR}/node_type_test.cpp
    ${PIPE_SRC_DIR}/node_io_test.cpp
    ${PIPE_SRC_DIR}/node_output_test.cpp
    ${PIPE_SRC_DIR}/crop_operation_test.cpp
    ${PIPE_SRC_DIR}/detector_operation_test.cpp
    ${PIPE_SRC_DIR}/pipe_descriptor_test.cpp
    ${PIPE_SRC_DIR}/pipe_config_test.cpp
    ${PIPE_SRC_DIR}/pipe_node_test.cpp
    ${PIPE_SRC_DIR}/pipe_test.cpp
    ${PIPE_SRC_DIR}/pipe_api_test.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${PIPE_SRC_DIR})
target_sources(${PROJECT_NAME} PRIVATE ${PIPE_SRC})
