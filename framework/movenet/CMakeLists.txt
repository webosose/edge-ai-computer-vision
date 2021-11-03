cmake_minimum_required(VERSION 2.8.7)

set(MOVENET_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(MOVENET_SRC
    ${MOVENET_SRC_DIR}/MovenetParam.cpp
    ${MOVENET_SRC_DIR}/MovenetParam.cpp
    ${MOVENET_SRC_DIR}/MovenetDetector.cpp
    ${MOVENET_SRC_DIR}/CpuMovenetDetector.cpp
    ${MOVENET_SRC_DIR}/MovenetDetectorFactory.cpp
    ${MOVENET_SRC_DIR}/MovenetDescriptor.cpp
)   

IF(WITH_EDGETPU)
    set(MOVENET_EDGETPU_SRC
        ${MOVENET_SRC_DIR}/EdgeTpuMovenetDetector.cpp
    )
ENDIF(WITH_EDGETPU)

target_include_directories(${PROJECT_NAME} PRIVATE ${MOVENET_SRC_DIR})
target_sources(${PROJECT_NAME} PRIVATE ${MOVENET_SRC} ${MOVENET_EDGETPU_SRC})
