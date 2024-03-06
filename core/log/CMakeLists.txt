cmake_minimum_required(VERSION 2.8.7)

set(LOG_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(LOG_SRC
    ${LOG_SRC_DIR}/Logger.cpp
    ${LOG_SRC_DIR}/LogMessage.cpp
)

target_include_directories(${CORE_NAME} PRIVATE ${LOG_SRC_DIR})
target_sources(${CORE_NAME} PRIVATE ${LOG_SRC})
