cmake_minimum_required(VERSION 3.10)
project(pipelinelib_test)

set(CMAKE_CXX_STANDARD 14)

add_executable(${PROJECT_NAME}
        src/main.cpp
        src/PipelineOperationTest.cpp
        src/ObserverTest.cpp
        src/AdvancedNodeTest.cpp
        src/InputAdapterTest.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/3rd_party)
target_link_libraries(${PROJECT_NAME} pipelinelib)