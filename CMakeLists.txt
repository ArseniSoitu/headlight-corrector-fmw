cmake_minimum_required(VERSION 3.00)

project(headlight_corrector C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CMAKE_EXPORT_COMPILE_COMMANDS)
  set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
  set(CMAKE_C_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_C_IMPLICIT_INCLUDE_DIRECTORIES})
endif()


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
    ${CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES}
    ${CMAKE_C_STANDARD_INCLUDE_DIRECTORIES}
)
