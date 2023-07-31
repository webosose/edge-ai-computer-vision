cmake_minimum_required(VERSION 2.8.7)

set(TEXT_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(TEXT_SRC
    ${TEXT_SRC_DIR}/TextParam.cpp
    ${TEXT_SRC_DIR}/TextDetector.cpp
    ${TEXT_SRC_DIR}/CpuTextDetector.cpp
    ${TEXT_SRC_DIR}/TextDescriptor.cpp
)

IF(WITH_EDGETPU)
    set(TEXT_EDGETPU_SRC
    )
ENDIF(WITH_EDGETPU)

target_include_directories(${PROJECT_NAME} PRIVATE ${TEXT_SRC_DIR})
target_sources(${PROJECT_NAME} PRIVATE ${TEXT_SRC} ${TEXT_EDGETPU_SRC})
