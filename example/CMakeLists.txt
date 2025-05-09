cmake_minimum_required(VERSION 2.8.7)
project (edgeai-vision)

IF("${AIF_INSTALL_EXAMPLE_DIR}" STREQUAL "")
    set(AIF_INSTALL_EXAMPLE_DIR "${AIF_INSTALL_DIR}/example")
ENDIF()

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/tools)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/apis/face)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/apis/websocket-service)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/apis/dl-sample)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/apis/text)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/apis/memory-sample)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/apis/sign-language-area)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/pipe-sample)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/face-detect-sample)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/posenet-sample)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/dynamic-loading-sample)

# for Extra Models
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/face-websocket-server)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/poselandmark-sample)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/handlandmark-sample)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/palm-sample)

file(GLOB IMAGES "${CMAKE_CURRENT_LIST_DIR}/images/*")
install(FILES ${IMAGES} DESTINATION ${AIF_INSTALL_EXAMPLE_DIR}/images/)
