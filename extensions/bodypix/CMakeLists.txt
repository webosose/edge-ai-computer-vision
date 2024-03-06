cmake_minimum_required(VERSION 2.8.7)

set(BODYPIX_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(BODYPIX_SRC
    ${BODYPIX_SRC_DIR}/BodypixParam.cpp
    ${BODYPIX_SRC_DIR}/BodypixParam.cpp
    ${BODYPIX_SRC_DIR}/BodypixDetector.cpp
    ${BODYPIX_SRC_DIR}/BodypixDescriptor.cpp
    ${BODYPIX_SRC_DIR}/CpuBodypixDetector.cpp
)   

IF(WITH_EDGETPU)
    set(BODYPIX_EDGETPU_SRC
        ${BODYPIX_SRC_DIR}/EdgeTpuBodypixDetector.cpp
    )
ENDIF(WITH_EDGETPU)

target_include_directories(${EXTENSION_NAME} PRIVATE ${BODYPIX_SRC_DIR})
target_sources(${EXTENSION_NAME} PRIVATE ${BODYPIX_SRC} ${BODYPIX_EDGETPU_SRC})
