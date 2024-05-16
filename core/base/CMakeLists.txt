cmake_minimum_required(VERSION 2.8.7)

set(BASE_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(BASE_SRC
    ${BASE_SRC_DIR}/AIVision.cpp
    ${BASE_SRC_DIR}/Descriptor.cpp
    ${BASE_SRC_DIR}/DetectorParam.cpp
    ${BASE_SRC_DIR}/Detector.cpp
    ${BASE_SRC_DIR}/DetectorFactory.cpp
    ${BASE_SRC_DIR}/Delegate.cpp
    ${BASE_SRC_DIR}/DelegateFactory.cpp
    ${BASE_SRC_DIR}/ExtensionLoader.cpp
    ${BASE_SRC_DIR}/SolutionFactory.cpp
)

target_include_directories(${CORE_NAME} PRIVATE ${BASE_SRC_DIR})
target_sources(${CORE_NAME} PRIVATE ${BASE_SRC})
