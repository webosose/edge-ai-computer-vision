cmake_minimum_required(VERSION 2.8.7)

set(YOLOV3_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(YOLOV3_SRC
    ${YOLOV3_SRC_DIR}/Yolov3Param.cpp
    ${YOLOV3_SRC_DIR}/Yolov3Detector.cpp
    ${YOLOV3_SRC_DIR}/Yolov3Detectors.cpp
    ${YOLOV3_SRC_DIR}/Yolov3Descriptor.cpp
    ${YOLOV3_SRC_DIR}/Yolov3PTQDetector.cpp
)

IF(WITH_EDGETPU)
    set(YOLOV3_EDGETPU_SRC
    )
ENDIF(WITH_EDGETPU)

target_include_directories(${PROJECT_NAME} PRIVATE ${YOLOV3_SRC_DIR})
target_sources(${PROJECT_NAME} PRIVATE ${YOLOV3_SRC} ${YOLOV3_EDGETPU_SRC})
