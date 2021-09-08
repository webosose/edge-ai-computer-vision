cmake_minimum_required (VERSION 2.6)
project (ai-framework)

set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

add_subdirectory(framework)
add_subdirectory(test)

add_dependencies(ai-framework-test ai-framework)
