cmake_minimum_required(VERSION 2.8.7)

set(PLUGIN_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(PLUGIN_SRC
    ${PLUGIN_SRC_DIR}/init.cpp
)

target_include_directories(${EXTENSION_NAME} PRIVATE ${PLUGIN_SRC_DIR})
target_sources(${EXTENSION_NAME} PRIVATE ${PLUGIN_SRC})
