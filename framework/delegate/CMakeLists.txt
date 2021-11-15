cmake_minimum_required(VERSION 2.8.7)

set(DELEGATE_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(DELEGATE_SRC
    ${DELEGATE_SRC_DIR}/ArmNNDelegate.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${DELEGATE_SRC_DIR})
target_sources(${PROJECT_NAME} PRIVATE ${DELEGATE_SRC})
