
cmake_minimum_required(VERSION 3.5)
project(js_test)

include_directories(${CMAKE_BINARY_DIR})
include_directories("../robot/include")
include_directories("./")
include_directories("../common/include/")

file(GLOB sources "*.cpp")

add_executable(js_test ${sources})

target_link_libraries(js_test pthread robot)
if(CMAKE_SYSTEM_NAME MATCHES Linux)
    target_link_libraries(js_test rt)
endif()
