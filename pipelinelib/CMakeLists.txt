cmake_minimum_required(VERSION 3.10)
project(pipelinelib)

set(CMAKE_CXX_STANDARD 14)

add_library(${PROJECT_NAME} SHARED
        src/NodeAlgorithms.cpp
        src/NodeExecution.cpp
        src/PipelineException.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/incl)