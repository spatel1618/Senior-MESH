cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(testv2_project C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

add_executable(testv2
  testv2.c
)

pico_enable_stdio_usb(testv2 1)
pico_enable_stdio_uart(testv2 1)

pico_add_extra_outputs(testv2)

target_link_libraries(testv2 pico_stdlib)
target_link_libraries(testv2 hardware_adc)