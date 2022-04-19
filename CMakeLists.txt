cmake_minimum_required (VERSION 2.6)
project (edgeai-vision)

set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

add_subdirectory(framework)
add_subdirectory(test)
add_subdirectory(example/face-websocket-server)
add_subdirectory(example/face-detect-sample)
add_subdirectory(example/posenet-sample)
add_subdirectory(example/poselandmark-sample)
add_subdirectory(example/handlandmark-sample)
add_subdirectory(example/palm-sample)
add_subdirectory(example/apis/face)

add_dependencies(edgeai-vision-test edgeai-vision)
add_dependencies(face-websocket-server edgeai-vision)
add_dependencies(face-detect-sample edgeai-vision)
