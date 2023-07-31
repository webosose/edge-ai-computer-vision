cmake_minimum_required(VERSION 2.8.7)

set(SELFIE_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(SELFIE_SRC
    ${SELFIE_SRC_DIR}/SelfieParam.cpp
    ${SELFIE_SRC_DIR}/SelfieParam.cpp
    ${SELFIE_SRC_DIR}/SelfieDetector.cpp
    ${SELFIE_SRC_DIR}/CpuSelfieDetector.cpp
    ${SELFIE_SRC_DIR}/SelfieDescriptor.cpp
)

IF(WITH_EDGETPU)
    #    set(SELFIE_EDGETPU_SRC
    #        ${SELFIE_SRC_DIR}/EdgeTpuSelfieDetector.cpp
    #    )
ENDIF(WITH_EDGETPU)

target_include_directories(${PROJECT_NAME} PRIVATE ${SELFIE_SRC_DIR})
target_sources(${PROJECT_NAME} PRIVATE ${SELFIE_SRC} ${SELFIE_EDGETPU_SRC})
