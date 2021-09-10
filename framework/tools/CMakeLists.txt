cmake_minimum_required(VERSION 2.8.7)

set(TOOLS_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(TOOLS_SRC
    ${TOOLS_SRC_DIR}/Stopwatch.cpp
    ${TOOLS_SRC_DIR}/ConfigReader.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${TOOLS_SRC_DIR})
target_sources(${PROJECT_NAME} PRIVATE ${TOOLS_SRC})
