cmake_minimum_required(VERSION 2.8.7)

set(HANDLANDMARK_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(HANDLANDMARK_SRC
    ${HANDLANDMARK_SRC_DIR}/HandLandmarkParam.cpp
    ${HANDLANDMARK_SRC_DIR}/HandLandmarkParam.cpp
    ${HANDLANDMARK_SRC_DIR}/HandLandmarkDetector.cpp
    ${HANDLANDMARK_SRC_DIR}/CpuHandLandmarkDetector.cpp
    ${HANDLANDMARK_SRC_DIR}/HandLandmarkDescriptor.cpp
)   

IF(WITH_EDGETPU)
    #    set(HANDLANDMARK_EDGETPU_SRC
    #        ${HANDLANDMARK_SRC_DIR}/EdgeTpuHandLandmarkDetector.cpp
    #    )
ENDIF(WITH_EDGETPU)

target_include_directories(${EXTENSION_NAME} PRIVATE ${HANDLANDMARK_SRC_DIR})
target_sources(${EXTENSION_NAME} PRIVATE ${HANDLANDMARK_SRC} ${HANDLANDMARK_EDGETPU_SRC})
