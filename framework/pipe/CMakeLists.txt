cmake_minimum_required(VERSION 2.8.7)

set(PIPE_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(PIPE_SRC
    ${PIPE_SRC_DIR}/NodeType.cpp
    ${PIPE_SRC_DIR}/NodeIO.cpp
    ${PIPE_SRC_DIR}/NodeInput.cpp
    ${PIPE_SRC_DIR}/NodeOutput.cpp
    ${PIPE_SRC_DIR}/NodeOperation.cpp
    ${PIPE_SRC_DIR}/CropOperation.cpp
    ${PIPE_SRC_DIR}/PalmCropOperation.cpp
    ${PIPE_SRC_DIR}/DetectorOperation.cpp
    ${PIPE_SRC_DIR}/NodeOperationFactory.cpp
    ${PIPE_SRC_DIR}/PipeConfig.cpp
    ${PIPE_SRC_DIR}/PipeDescriptor.cpp
    ${PIPE_SRC_DIR}/PipeNode.cpp
    ${PIPE_SRC_DIR}/Pipe.cpp
    ${PIPE_SRC_DIR}/PipeDescriptorFactory.cpp
    ${PIPE_SRC_DIR}/PersonCropOperation.cpp
    ${PIPE_SRC_DIR}/FaceCropOperation.cpp
    ${PIPE_SRC_DIR}/FaceMeshExtractOperation.cpp
    ${PIPE_SRC_DIR}/FaceMeshExtractOperationConfig.cpp
    ${PIPE_SRC_DIR}/ChangeInputOperation.cpp
    ${PIPE_SRC_DIR}/ChangeInputOperationConfig.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${PIPE_SRC_DIR})
target_sources(${PROJECT_NAME} PRIVATE ${PIPE_SRC})
