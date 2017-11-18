cmake_minimum_required(VERSION 3.8)
project(OscProxy)

add_subdirectory(oscpack oscpack)
set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp)
add_executable(OscProxy ${SOURCE_FILES})
target_link_libraries(OscProxy oscpack)