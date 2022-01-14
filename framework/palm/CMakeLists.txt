cmake_minimum_required(VERSION 2.8.7)

set(PALM_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(PALM_SRC
    ${PALM_SRC_DIR}/PalmParam.cpp
    ${PALM_SRC_DIR}/PalmParam.cpp
    ${PALM_SRC_DIR}/PalmDetector.cpp
    ${PALM_SRC_DIR}/CpuPalmDetector.cpp
    ${PALM_SRC_DIR}/PalmDescriptor.cpp
)   

IF(WITH_EDGETPU)
    set(PALM_EDGETPU_SRC
        ${PALM_SRC_DIR}/EdgeTpuPalmDetector.cpp
    )
ENDIF(WITH_EDGETPU)

target_include_directories(${PROJECT_NAME} PRIVATE ${PALM_SRC_DIR})
target_sources(${PROJECT_NAME} PRIVATE ${PALM_SRC} ${PALM_EDGETPU_SRC})
