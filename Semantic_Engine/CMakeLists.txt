cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 17)
project(SemanticEngine)



set(SCOURCE_FILES
    SemanticSearch.cpp
    ./glove/glove.c)

# Set the build output location to a common directory
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)

add_executable(SemanticSearch ${SCOURCE_FILES})

target_include_directories(SemanticSearch PRIVATE ./glove)

target_compile_options(SemanticSearch PRIVATE -Wall -Wextra -Wpedantic)