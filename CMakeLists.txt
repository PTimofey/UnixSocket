cmake_minimum_required(VERSION 3.5)

project(Test)

find_package(Threads)

add_executable(Programm1 Programm1.cpp)

set_target_properties(
    Programm1 PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
)

target_link_libraries(
    Programm1
    ${CMAKE_THREAD_LIBS_INIT}
)

add_executable(Programm2 Programm2.cpp)

set_target_properties(
    Programm2 PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
)