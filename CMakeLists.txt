cmake_minimum_required(VERSION 3.10)
project(YamlParser)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(Catch2/)

add_executable(
    yaml-parser-tests 
    test/test.cpp
)
target_link_libraries(
    yaml-parser-tests 
    PRIVATE 
    Catch2::Catch2WithMain)
target_include_directories(
    yaml-parser-tests 
    PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/Catch2/src/
    ${CMAKE_CURRENT_SOURCE_DIR}/include/)
