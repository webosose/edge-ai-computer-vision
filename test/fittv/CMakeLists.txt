cmake_minimum_required(VERSION 2.8.7)

set(FITTV_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(FITTV_SRC
    ${FITTV_SRC_DIR}/fittv_descriptor_test.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${FITTV_SRC_DIR})
target_sources(${PROJECT_NAME} PRIVATE ${FITTV_SRC})
