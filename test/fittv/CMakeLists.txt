cmake_minimum_required(VERSION 2.8.7)

set(FITTV_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(FITTV_SRC
    ${FITTV_SRC_DIR}/fittv_descriptor_test.cpp
    ${FITTV_SRC_DIR}/bodyposeestimation_param_test.cpp
    ${FITTV_SRC_DIR}/fittv_descriptor_test.cpp
    ${FITTV_SRC_DIR}/pose2d_descriptor_test.cpp
    ${FITTV_SRC_DIR}/pose2d_detector_test.cpp
    ${FITTV_SRC_DIR}/pose3d_descriptor_test.cpp
    ${FITTV_SRC_DIR}/pose3d_detector_test.cpp
    ${FITTV_SRC_DIR}/pose3d_param_test.cpp
    ${FITTV_SRC_DIR}/yolov3_descriptor_test.cpp
    ${FITTV_SRC_DIR}/yolov3_detector_test.cpp
    ${FITTV_SRC_DIR}/yolov3_param_test.cpp
    ${FITTV_SRC_DIR}/yolov4_descriptor_test.cpp
    ${FITTV_SRC_DIR}/yolov4_detector_test.cpp
    ${FITTV_SRC_DIR}/yolov4_param_test.cpp
    ${FITTV_SRC_DIR}/one_euro_filter_operation_test.cpp
    ${FITTV_SRC_DIR}/bgsegment_detector_test.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${FITTV_SRC_DIR})
target_sources(${PROJECT_NAME} PRIVATE ${FITTV_SRC})
