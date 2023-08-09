cmake_minimum_required(VERSION 2.8.7)

set(FITTV_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(FITTV_SRC
    ${FITTV_SRC_DIR}/FitTvPoseDescriptor.cpp
    ${FITTV_SRC_DIR}/FitTvPersonCropOperation.cpp
    ${FITTV_SRC_DIR}/FitTvPose2dDetectorOperation.cpp
    ${FITTV_SRC_DIR}/FitTvPose3dDetectorOperation.cpp
    ${FITTV_SRC_DIR}/FitTvPose2d3dBridgeOperation.cpp
    ${FITTV_SRC_DIR}/FitTvOneEuroFilterOperation.cpp
    ${FITTV_SRC_DIR}/FitTvOneEuroFilterOperationConfig.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${FITTV_SRC_DIR})
target_sources(${PROJECT_NAME} PRIVATE ${FITTV_SRC})
