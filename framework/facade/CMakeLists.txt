cmake_minimum_required(VERSION 2.8.7)

set(BASE_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(BASE_SRC
    ${BASE_SRC_DIR}/EdgeAIVision.cpp
    ${BASE_SRC_DIR}/EdgeAIVisionC.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${BASE_SRC_DIR})
target_sources(${PROJECT_NAME} PRIVATE ${BASE_SRC})
