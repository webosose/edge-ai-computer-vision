cmake_minimum_required(VERSION 2.8.7)

set(YOLOV4_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(YOLOV4_SRC
    ${YOLOV4_SRC_DIR}/Yolov4Param.cpp
    ${YOLOV4_SRC_DIR}/Yolov4Detector.cpp
    ${YOLOV4_SRC_DIR}/NpuYolov4Detector.cpp
    ${YOLOV4_SRC_DIR}/Yolov4Descriptor.cpp
)

IF(WITH_EDGETPU)
    set(YOLOV4_EDGETPU_SRC
    )
ENDIF(WITH_EDGETPU)

target_include_directories(${PROJECT_NAME} PRIVATE ${YOLOV4_SRC_DIR})
target_sources(${PROJECT_NAME} PRIVATE ${YOLOV4_SRC} ${YOLOV4_EDGETPU_SRC})
