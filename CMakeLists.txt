cmake_minimum_required(VERSION 3.00)

set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

project(headlight_corrector C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(${PROJECT_NAME} 
    src/main.cpp 
    board/startup/handlers.cpp
    board/startup/stack.cpp
    board/startup/startup.cpp
    ${CMAKE_SOURCE_DIR}/platform/st/stm32/f0/STM32CubeF0/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_usart.c
    ${CMAKE_SOURCE_DIR}/platform/st/stm32/f0/STM32CubeF0/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_rcc.c
)
target_include_directories(${PROJECT_NAME} PUBLIC 
    ${CMAKE_SOURCE_DIR}/board/
    ${CMAKE_SOURCE_DIR}/platform/st/stm32/f0/STM32CubeF0/Drivers/CMSIS/Device/ST/STM32F0xx/Include/
    ${CMAKE_SOURCE_DIR}/platform/st/stm32/f0/STM32CubeF0/Drivers/CMSIS/Core/Include/
    ${CMAKE_SOURCE_DIR}/platform/st/stm32/f0/STM32CubeF0/Drivers/STM32F0xx_HAL_Driver/Inc/
    ${CMAKE_SOURCE_DIR}/platform/st/stm32/f0/STM32CubeF0/Drivers/STM32F0xx_HAL_Driver/Inc/Legacy/
)
