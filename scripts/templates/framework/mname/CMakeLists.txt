cmake_minimum_required(VERSION 2.8.7)

set(MNAME_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(MNAME_SRC
    ${MNAME_SRC_DIR}/MNameParam.cpp
    ${MNAME_SRC_DIR}/MNameParam.cpp
    ${MNAME_SRC_DIR}/MNameDetector.cpp
    ${MNAME_SRC_DIR}/CpuMNameDetector.cpp
    ${MNAME_SRC_DIR}/MNameDetectorFactory.cpp
    ${MNAME_SRC_DIR}/MNameDescriptor.cpp
)   

IF(WITH_EDGETPU)
    set(MNAME_EDGETPU_SRC
        ${MNAME_SRC_DIR}/EdgeTpuMNameDetector.cpp
    )
ENDIF(WITH_EDGETPU)

target_include_directories(${PROJECT_NAME} PRIVATE ${MNAME_SRC_DIR})
target_sources(${PROJECT_NAME} PRIVATE ${MNAME_SRC} ${MNAME_EDGETPU_SRC})
