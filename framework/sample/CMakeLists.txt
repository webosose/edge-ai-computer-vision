cmake_minimum_required(VERSION 2.8.7)

set(SAMPLE_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(SAMPLE_SRC
    ${SAMPLE_SRC_DIR}/Sample.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${SAMPLE_SRC_DIR})
target_sources(${PROJECT_NAME} PRIVATE ${SAMPLE_SRC})
