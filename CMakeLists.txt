cmake_minimum_required(VERSION 3.10)
project(YamlParser)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(Catch2/)

add_executable(
    yaml-parser-tests 
    test/test-parsing.cpp
    test/test-writing.cpp
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

add_test(
    NAME yaml-parser-tests
    COMMAND yaml-parser-tests --reporter "JUnit::out=../test-reports/yaml-parser-tests.xml" --reporter "console::out=-::colour-mode=ansi"
)

if (YAML_PARSER_ENABLE_FORMAT_LIB)
    target_compile_definitions(
        yaml-parser-tests 
        PRIVATE 
        YAML_PARSER_ENABLE_FORMAT_LIB)
endif()
