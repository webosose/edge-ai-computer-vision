cmake_minimum_required(VERSION 2.8.7)

set(RPPG_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(RPPG_SRC
    ${RPPG_SRC_DIR}/RppgParam.cpp
    ${RPPG_SRC_DIR}/RppgDetector.cpp
    ${RPPG_SRC_DIR}/CpuRppgDetector.cpp
    ${RPPG_SRC_DIR}/NpuRppgDetector.cpp
    ${RPPG_SRC_DIR}/RppgDescriptor.cpp
    ${RPPG_SRC_DIR}/CubicSpline.cpp
    ${RPPG_SRC_DIR}/FftLib.cpp
    ${RPPG_SRC_DIR}/RppgInferencePipeDescriptor.cpp
    ${RPPG_SRC_DIR}/RppgPostProcessOperation.cpp
    ${RPPG_SRC_DIR}/RppgPreProcessOperation.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${RPPG_SRC_DIR})
target_sources(${PROJECT_NAME} PRIVATE ${RPPG_SRC})
