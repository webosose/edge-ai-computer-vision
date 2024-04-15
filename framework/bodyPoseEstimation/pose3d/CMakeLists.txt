cmake_minimum_required(VERSION 2.8.7)

set(POSE3D_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(POSE3D_SRC
    ${POSE3D_SRC_DIR}/Pose3dParam.cpp
    ${POSE3D_SRC_DIR}/Pose3dParam.cpp
    ${POSE3D_SRC_DIR}/Pose3dDetector.cpp
    ${POSE3D_SRC_DIR}/Pose3dDetectors.cpp
    ${POSE3D_SRC_DIR}/Pose3dPTQDetector.cpp
    ${POSE3D_SRC_DIR}/Pose3dPTQDetectors.cpp
    ${POSE3D_SRC_DIR}/Pose3dDescriptor.cpp
)

IF(WITH_EDGETPU)
    set(POSE3D_EDGETPU_SRC
    )
ENDIF(WITH_EDGETPU)

target_include_directories(${PROJECT_NAME} PRIVATE ${POSE3D_SRC_DIR})
target_sources(${PROJECT_NAME} PRIVATE ${POSE3D_SRC} ${POSE3D_EDGETPU_SRC})
