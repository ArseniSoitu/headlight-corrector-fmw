cmake_minimum_required(VERSION 3.13)

# initialize the SDK directly
include(${CMAKE_SOURCE_DIR}/lib/pico-sdk/pico_sdk_init.cmake)

project(headlight_corrector)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

add_executable(${CMAKE_PROJECT_NAME}
    ${CMAKE_SOURCE_DIR}/src/main.c
)

pico_generate_pio_header(${CMAKE_PROJECT_NAME} ${CMAKE_SOURCE_DIR}/src/pio/square_wave.pio)

# Add pico_stdlib library which aggregates commonly used features
target_link_libraries(${CMAKE_PROJECT_NAME} pico_stdlib hardware_pio hardware_adc)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(${CMAKE_PROJECT_NAME})
