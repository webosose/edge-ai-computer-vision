cmake_minimum_required(VERSION 2.8.7)

set(POSELANDMARK_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(POSELANDMARK_SRC
    ${POSELANDMARK_SRC_DIR}/PoseLandmarkParam.cpp
    ${POSELANDMARK_SRC_DIR}/PoseLandmarkDetector.cpp
    ${POSELANDMARK_SRC_DIR}/CpuPoseLandmarkDetector.cpp
    ${POSELANDMARK_SRC_DIR}/PoseLandmarkDescriptor.cpp
)

IF(WITH_EDGETPU)
    set(POSELANDMARK_EDGETPU_SRC
        ${POSELANDMARK_SRC_DIR}/EdgeTpuPoseLandmarkDetector.cpp
    )
ENDIF(WITH_EDGETPU)

target_include_directories(${PROJECT_NAME} PRIVATE ${POSELANDMARK_SRC_DIR})
target_sources(${PROJECT_NAME} PRIVATE ${POSELANDMARK_SRC} ${POSELANDMARK_EDGETPU_SRC})
