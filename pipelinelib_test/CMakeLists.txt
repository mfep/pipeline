cmake_minimum_required(VERSION 3.10)
project(pipelinelib_test)

set(CMAKE_CXX_STANDARD 14)

add_executable(${PROJECT_NAME}
        main.cpp
        PipelineOperationTest.cpp
        ObserverTest.cpp
        AdvancedNodeTest.cpp)
target_link_libraries(${PROJECT_NAME} pipelinelib)