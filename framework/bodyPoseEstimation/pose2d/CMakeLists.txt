cmake_minimum_required(VERSION 2.8.7)

set(POSE2D_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(POSE2D_SRC
    ${POSE2D_SRC_DIR}/Pose2dParam.cpp
    ${POSE2D_SRC_DIR}/Pose2dParam.cpp
    ${POSE2D_SRC_DIR}/Pose2dDetector.cpp
    ${POSE2D_SRC_DIR}/Pose2dDescriptor.cpp
    ${POSE2D_SRC_DIR}/CpuPose2dDetector.cpp
    ${POSE2D_SRC_DIR}/CpuPose2dDetectors.cpp
    ${POSE2D_SRC_DIR}/NpuPose2dDetector.cpp
    ${POSE2D_SRC_DIR}/NpuPose2dDetectors.cpp
    ${POSE2D_SRC_DIR}/Pose2dPTQDetector.cpp
    ${POSE2D_SRC_DIR}/Pose2dPTQDetectors.cpp
    ${POSE2D_SRC_DIR}/CpuQuantPose2dDetector.cpp
    ${POSE2D_SRC_DIR}/RegularPostProcess.cpp
    ${POSE2D_SRC_DIR}/XtensorPostProcess.cpp
    ${POSE2D_SRC_DIR}/PostProcess.cpp
)

IF(WITH_EDGETPU)
    set(POSE2D_EDGETPU_SRC
    )
ENDIF(WITH_EDGETPU)

target_include_directories(${PROJECT_NAME} PRIVATE ${POSE2D_SRC_DIR})
target_sources(${PROJECT_NAME} PRIVATE ${POSE2D_SRC} ${POSE2D_EDGETPU_SRC})
