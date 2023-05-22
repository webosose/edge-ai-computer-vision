cmake_minimum_required(VERSION 2.8.7)

set(FACEMESH_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(FACEMESH_SRC
    ${FACEMESH_SRC_DIR}/FaceMeshParam.cpp
    ${FACEMESH_SRC_DIR}/FaceMeshDetector.cpp
    ${FACEMESH_SRC_DIR}/CpuFaceMeshDetector.cpp
    ${FACEMESH_SRC_DIR}/FaceMeshDescriptor.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${FACEMESH_SRC_DIR})
target_sources(${PROJECT_NAME} PRIVATE ${FACEMESH_SRC})
