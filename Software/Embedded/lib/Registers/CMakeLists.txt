# Set minimum required version of CMake
cmake_minimum_required(VERSION 3.15)

# Set the project name
project(Registers)

# Add the library with the above sources
add_library(${PROJECT_NAME} src/Registers.cpp)
add_library(sub::Registers ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)