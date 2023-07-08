# Set minimum required version of CMake
cmake_minimum_required(VERSION 3.15)

# Set the project name
project(PicoGFX)

# Add the library with the above sources
add_library(${PROJECT_NAME} 
    src/HardwareSPI.cpp
    src/Display.cpp
    src/Print.cpp
    src/Graphics.cpp
    src/Gradients.cpp
    src/Dials.cpp
    src/PicoGFX.cpp
    src/GfxMath.c
    Specifics/ST7789.cpp
    Specifics/GC9A01.cpp
)

add_library(sub::Display ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}/include
    PUBLIC ${PROJECT_SOURCE_DIR}/include/Extras
    PUBLIC ${PROJECT_SOURCE_DIR}/Fonts
)

pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/include/pio_spi.pio)

target_link_libraries(${PROJECT_NAME} 
    pico_stdlib
    hardware_spi
    hardware_dma
    hardware_pwm
    hardware_pio
    pico_multicore
)
