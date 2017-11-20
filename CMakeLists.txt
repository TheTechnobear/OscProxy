cmake_minimum_required(VERSION 3.8)
project(OscProxy)

add_subdirectory(oscpack oscpack)
set(CMAKE_CXX_STANDARD 11)

if(MSVC)
    set(CompilerFlags
            CMAKE_CXX_FLAGS
            CMAKE_CXX_FLAGS_DEBUG
            CMAKE_CXX_FLAGS_RELEASE
            CMAKE_C_FLAGS
            CMAKE_C_FLAGS_DEBUG
            CMAKE_C_FLAGS_RELEASE
            )
    foreach(CompilerFlag ${CompilerFlags})
        string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
    endforeach()
endif()

set(SOURCE_FILES main.cpp)
add_executable(OscProxy ${SOURCE_FILES})
target_link_libraries(OscProxy oscpack)