cmake_minimum_required(VERSION 2.8.7)

set(YOLOV3_V2_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(YOLOV3_V2_SRC
    ${YOLOV3_V2_SRC_DIR}/Yolov3V2Param.cpp
    ${YOLOV3_V2_SRC_DIR}/Yolov3V2Detector.cpp
    ${YOLOV3_V2_SRC_DIR}/NpuYolov3V2Detector.cpp
    ${YOLOV3_V2_SRC_DIR}/Yolov3V2Descriptor.cpp
)

IF(WITH_EDGETPU)
    set(YOLOV3_V2_EDGETPU_SRC
    )
ENDIF(WITH_EDGETPU)

target_include_directories(${PROJECT_NAME} PRIVATE ${YOLOV3_V2_SRC_DIR})
target_sources(${PROJECT_NAME} PRIVATE ${YOLOV3_V2_SRC} ${YOLOV3_V2_EDGETPU_SRC})
