cmake_minimum_required(VERSION 2.8.7)

set(FACE_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(FACE_SRC
    ${FACE_SRC_DIR}/FaceDescriptor.cpp
    ${FACE_SRC_DIR}/FaceParam.cpp
    ${FACE_SRC_DIR}/FaceDetector.cpp
    ${FACE_SRC_DIR}/CpuFaceDetector.cpp
)

IF(WITH_EDGETPU)
    set(FACE_EDGETPU_SRC
        ${FACE_SRC_DIR}/EdgeTpuFaceDetector.cpp
    )
ENDIF(WITH_EDGETPU)

target_include_directories(${PROJECT_NAME} PRIVATE ${FACE_SRC_DIR})
target_sources(${PROJECT_NAME} PRIVATE ${FACE_SRC} ${FACE_EDGETPU_SRC})
