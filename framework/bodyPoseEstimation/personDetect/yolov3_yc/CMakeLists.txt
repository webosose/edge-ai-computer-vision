cmake_minimum_required(VERSION 2.8.7)

set(YOLOV3_YC_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(YOLOV3_YC_SRC
    ${YOLOV3_YC_SRC_DIR}/Yolov3YCParam.cpp
    ${YOLOV3_YC_SRC_DIR}/Yolov3YCDetector.cpp
    ${YOLOV3_YC_SRC_DIR}/NpuYolov3YCDetector.cpp
    ${YOLOV3_YC_SRC_DIR}/Yolov3YCDescriptor.cpp
)

IF(WITH_EDGETPU)
    set(YOLOV3_YC_EDGETPU_SRC
    )
ENDIF(WITH_EDGETPU)

target_include_directories(${PROJECT_NAME} PRIVATE ${YOLOV3_YC_SRC_DIR})
target_sources(${PROJECT_NAME} PRIVATE ${YOLOV3_YC_SRC} ${YOLOV3_YC_EDGETPU_SRC})
