cmake_minimum_required(VERSION 2.8.7)

set(BODYPOSEESTIMATION_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(BODYPOSEESTIMATION_SRC
    ${BODYPOSEESTIMATION_SRC_DIR}/BodyPoseEstimationParam.cpp
    ${BODYPOSEESTIMATION_SRC_DIR}/common.cpp
)


add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/yolov4")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/pose2d")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/fittv")

target_include_directories(${PROJECT_NAME} PRIVATE ${BODYPOSEESTIMATION_SRC_DIR})
target_sources(${PROJECT_NAME} PRIVATE ${BODYPOSEESTIMATION_SRC})
