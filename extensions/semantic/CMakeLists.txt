cmake_minimum_required(VERSION 2.8.7)

set(SEMANTIC_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(SEMANTIC_SRC
    ${SEMANTIC_SRC_DIR}/SemanticParam.cpp
    ${SEMANTIC_SRC_DIR}/SemanticParam.cpp
    ${SEMANTIC_SRC_DIR}/SemanticDetector.cpp
    ${SEMANTIC_SRC_DIR}/SemanticDescriptor.cpp
    ${SEMANTIC_SRC_DIR}/CpuSemanticDetector.cpp
)   

IF(WITH_EDGETPU)
    set(SEMANTIC_EDGETPU_SRC
        ${SEMANTIC_SRC_DIR}/EdgeTpuSemanticDetector.cpp
    )
ENDIF(WITH_EDGETPU)

target_include_directories(${EXTENSION_NAME} PRIVATE ${SEMANTIC_SRC_DIR})
target_sources(${EXTENSION_NAME} PRIVATE ${SEMANTIC_SRC} ${SEMANTIC_EDGETPU_SRC})
