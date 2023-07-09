# Set minimum required version of CMake
cmake_minimum_required(VERSION 3.15)

# Set the project name
project(INA219)

# Add the library with the above sources
add_library(${PROJECT_NAME} src/INA219.cpp)
add_library(sub::INA219 ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME} 
    pico_stdlib
    hardware_i2c
)