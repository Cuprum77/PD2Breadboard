# Set minimum required version of CMake
cmake_minimum_required(VERSION 3.15)

# Include build functions from Pico SDK
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)
include($ENV{PICO_SDK_PATH}/tools/CMakeLists.txt)

# Set name of project (as PROJECT_NAME) and C/C   standards
project(USB-PD_Power_Supply C CXX)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Creates a pico-sdk subdirectory in our project for the libraries
pico_sdk_init()

# Tell CMake where to find the executable source file
add_executable(${PROJECT_NAME} 
    ${CMAKE_CURRENT_LIST_DIR}/main.cpp
)

AUX_SOURCE_DIRECTORY(lib SUB_SOURCES)

target_sources(${PROJECT_NAME} PUBLIC 
    ${SUB_SOURCES}
)

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)

add_subdirectory(${CMAKE_SOURCE_DIR}/lib/Button)
add_subdirectory(${CMAKE_SOURCE_DIR}/lib/Display)
add_subdirectory(${CMAKE_SOURCE_DIR}/lib/FUSB302)
add_subdirectory(${CMAKE_SOURCE_DIR}/lib/INA219)
add_subdirectory(${CMAKE_SOURCE_DIR}/lib/Memory)

link_directories(${CMAKE_SOURCE_DIR}/lib/Button)
link_directories(${CMAKE_SOURCE_DIR}/lib/Display)
link_directories(${CMAKE_SOURCE_DIR}/lib/FUSB302)
link_directories(${CMAKE_SOURCE_DIR}/lib/INA219)
link_directories(${CMAKE_SOURCE_DIR}/lib/Memory)

# Create map/bin/hex/uf2 files
pico_add_extra_outputs(${PROJECT_NAME})

# Link to pico_stdlib (gpio, time, etc. functions)
target_link_libraries(${PROJECT_NAME} 
    pico_stdlib
    hardware_i2c
    Button
    Display
    FUSB302
    INA219
    Memory
)

# Enable usb output, disable uart output
pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)