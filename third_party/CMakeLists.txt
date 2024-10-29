cmake_minimum_required(VERSION 2.8.7)

set(THIRD_PARTY_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(THIRD_PARTY_SRC
    ${THIRD_PARTY_SRC_DIR}/clipper/clipper.cpp
    ${THIRD_PARTY_SRC_DIR}/posenet/posenet_decoder.cc
    ${THIRD_PARTY_SRC_DIR}/posenet/posenet_decoder_op.cc
    ${THIRD_PARTY_SRC_DIR}/mediapipe/selfie/transpose_conv_bias.cpp
    ${THIRD_PARTY_SRC_DIR}/CubicSpline/CubicSpline.cpp
)

#install(DIRECTORY
 #       ${CMAKE_SOURCE_DIR}/third_party
  #  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

file(GLOB THIRD_PARTY_HDRS
    "${THIRD_PARTY_SRC_DIR}/clipper/*.hpp"
    "${THIRD_PARTY_SRC_DIR}/posenet/*.h"
    "${THIRD_PARTY_SRC_DIR}/mediapipe/selfie/*.h"
    "${THIRD_PARTY_SRC_DIR}/CubicSpline/*.h"
)
install(FILES ${THIRD_PARTY_HDRS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/aif/third_party)


target_include_directories(${PROJECT_NAME} PRIVATE ${THIRD_PARTY_SRC_DIR})
target_sources(${PROJECT_NAME} PRIVATE ${THIRD_PARTY_SRC})
