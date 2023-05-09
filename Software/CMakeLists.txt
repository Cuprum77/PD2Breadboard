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
    ${CMAKE_CURRENT_LIST_DIR}/src/main.cpp
)

# Add all the source files in the lib directory to the project
AUX_SOURCE_DIRECTORY(lib SUB_SOURCES)

# Add the source files to the project
target_sources(${PROJECT_NAME} PUBLIC 
    ${SUB_SOURCES}
)

# Add the include directories to the project
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)

# Get the current working branch
execute_process(
  COMMAND git rev-parse --abbrev-ref HEAD
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_BRANCH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Get the latest abbreviated commit hash of the working branch
execute_process(
  COMMAND git log -1 --format=%h
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_COMMIT_HASH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

add_definitions("-DGIT_COMMIT_HASH=${GIT_COMMIT_HASH}")
add_definitions("-DGIT_BRANCH=${GIT_BRANCH}")

configure_file(
  ${CMAKE_SOURCE_DIR}/include/version.h.in
  ${CMAKE_BINARY_DIR}/generated/version.h
)

include_directories(${CMAKE_BINARY_DIR}/generated)
include_directories(${CMAKE_BINARY_DIR}/include)

add_subdirectory(${CMAKE_SOURCE_DIR}/lib/Button)
add_subdirectory(${CMAKE_SOURCE_DIR}/lib/Display)
add_subdirectory(${CMAKE_SOURCE_DIR}/lib/FUSB302)
add_subdirectory(${CMAKE_SOURCE_DIR}/lib/USB_PD)
add_subdirectory(${CMAKE_SOURCE_DIR}/lib/INA219)
add_subdirectory(${CMAKE_SOURCE_DIR}/lib/Memory)

link_directories(${CMAKE_SOURCE_DIR}/lib/Button)
link_directories(${CMAKE_SOURCE_DIR}/lib/Display)
link_directories(${CMAKE_SOURCE_DIR}/lib/FUSB302)
link_directories(${CMAKE_SOURCE_DIR}/lib/USB_PD)
link_directories(${CMAKE_SOURCE_DIR}/lib/INA219)
link_directories(${CMAKE_SOURCE_DIR}/lib/Memory)

# Create map/bin/hex/uf2 files
pico_add_extra_outputs(${PROJECT_NAME})

# Link to pico_stdlib (gpio, time, etc. functions)
target_link_libraries(${PROJECT_NAME} 
    pico_stdlib
    pico_multicore
    hardware_i2c
    Button
    Display
    USB_PD
    INA219
    Memory
)

# Enable usb output, disable uart output
pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)

# Run the programming script to program the device
# add_custom_command(TARGET ${PROJECT_NAME}
#     POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E echo "Programming device..."
#     COMMAND python3 ${CMAKE_SOURCE_DIR}/scripts/programmer/programmer.py
#     DEPENDS ${CMAKE_SOURCE_DIR}/scripts/programmer/programmer.py 
#     DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.u2f
#     FORCE
# )