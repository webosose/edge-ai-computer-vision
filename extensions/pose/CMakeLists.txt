cmake_minimum_required(VERSION 2.8.7)

set(POSENET_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(POSENET_SRC
    ${POSENET_SRC_DIR}/PosenetParam.cpp
    ${POSENET_SRC_DIR}/PosenetDetector.cpp
    ${POSENET_SRC_DIR}/CpuPosenetDetector.cpp
    ${POSENET_SRC_DIR}/PosenetDescriptor.cpp
)

IF(WITH_EDGETPU)
    set(POSENET_EDGETPU_SRC
        ${POSENET_SRC_DIR}/EdgeTpuPosenetDetector.cpp
    )
ENDIF(WITH_EDGETPU)

#IF(WITH_ARMNN)
#    set(POSENET_ARMNN_SRC
#        ${POSENET_SRC_DIR}/ArmNNPosenetDetector.cpp
#    )
#ENDIF(WITH_ARMNN)


target_include_directories(${EXTENSION_NAME} PRIVATE ${POSENET_SRC_DIR})
target_sources(${EXTENSION_NAME} PRIVATE ${POSENET_SRC} ${POSENET_EDGETPU_SRC} ${POSENET_ARMNN_SRC})
