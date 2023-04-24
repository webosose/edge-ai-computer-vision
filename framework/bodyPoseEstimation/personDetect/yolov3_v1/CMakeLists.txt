cmake_minimum_required(VERSION 2.8.7)

set(YOLOV3_V1_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(YOLOV3_V1_SRC
    ${YOLOV3_V1_SRC_DIR}/Yolov3V1Param.cpp
    ${YOLOV3_V1_SRC_DIR}/Yolov3V1Detector.cpp
    ${YOLOV3_V1_SRC_DIR}/NpuYolov3V1Detector.cpp
    ${YOLOV3_V1_SRC_DIR}/Yolov3V1Descriptor.cpp
)

IF(WITH_EDGETPU)
    set(YOLOV3_V1_EDGETPU_SRC
    )
ENDIF(WITH_EDGETPU)

target_include_directories(${PROJECT_NAME} PRIVATE ${YOLOV3_V1_SRC_DIR})
target_sources(${PROJECT_NAME} PRIVATE ${YOLOV3_V1_SRC} ${YOLOV3_V1_EDGETPU_SRC})
