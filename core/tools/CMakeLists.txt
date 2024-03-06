cmake_minimum_required(VERSION 2.8.7)

set(TOOLS_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(TOOLS_SRC
    ${TOOLS_SRC_DIR}/Stopwatch.cpp
    ${TOOLS_SRC_DIR}/ConfigReader.cpp
    ${TOOLS_SRC_DIR}/Utils.cpp
    ${TOOLS_SRC_DIR}/CAPIForTas.cpp
    ${TOOLS_SRC_DIR}/PerformanceReporter.cpp
)

target_include_directories(${CORE_NAME} PRIVATE ${TOOLS_SRC_DIR})
target_sources(${CORE_NAME} PRIVATE ${TOOLS_SRC})
