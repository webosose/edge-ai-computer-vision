cmake_minimum_required (VERSION 2.6)
project (ai-framework)

set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

add_subdirectory(framework)
add_subdirectory(test)
add_subdirectory(example/face-detect-sample)

add_dependencies(ai-framework-test ai-framework)
add_dependencies(face-detect-sample ai-framework)