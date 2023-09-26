cmake_minimum_required(VERSION 2.8.7)

set(BGSEGMENT_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(BGSEGMENT_SRC
    ${BGSEGMENT_SRC_DIR}/BgSegmentParam.cpp
    ${BGSEGMENT_SRC_DIR}/BgSegmentDetector.cpp
    ${BGSEGMENT_SRC_DIR}/BgSegmentDescriptor.cpp
#    ${BGSEGMENT_SRC_DIR}/CpuBgSegmentDetector.cpp
)

IF(WITH_NPU)
    set(BGSEGMENT_NPU_SRC
        ${BGSEGMENT_SRC_DIR}/NpuBgSegmentDetector.cpp
    )
ENDIF(WITH_NPU)

target_include_directories(${PROJECT_NAME} PRIVATE ${BGSEGMENT_SRC_DIR})
target_sources(${PROJECT_NAME} PRIVATE ${BGSEGMENT_SRC} ${BGSEGMENT_NPU_SRC})
