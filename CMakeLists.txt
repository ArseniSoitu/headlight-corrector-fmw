cmake_minimum_required(VERSION 3.00)

project(headlight_corrector C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CMAKE_EXPORT_COMPILE_COMMANDS)
  set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
  set(CMAKE_C_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_C_IMPLICIT_INCLUDE_DIRECTORIES})
endif()


add_executable(${PROJECT_NAME}
    ${CMAKE_SOURCE_DIR}/src/main.cpp
    ${CMAKE_SOURCE_DIR}/board/startup/handlers.cpp
    ${CMAKE_SOURCE_DIR}/board/startup/stack.cpp
    ${CMAKE_SOURCE_DIR}/board/startup/startup.cpp
    ${CMAKE_SOURCE_DIR}/board/board.cpp
    ${CMAKE_SOURCE_DIR}/board/system.cpp
    ${CMAKE_SOURCE_DIR}/board/serial.cpp
    ${CMAKE_SOURCE_DIR}/platform/st/stm32/f0/STM32CubeF0/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_usart.c
    ${CMAKE_SOURCE_DIR}/platform/st/stm32/f0/STM32CubeF0/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_gpio.c
    ${CMAKE_SOURCE_DIR}/platform/st/stm32/f0/STM32CubeF0/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_rcc.c
    ${CMAKE_SOURCE_DIR}/platform/st/stm32/f0/STM32CubeF0/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_utils.c
    ${CMAKE_SOURCE_DIR}/platform/st/stm32/f0/STM32CubeF0/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_tim.c
    ${CMAKE_SOURCE_DIR}/platform/st/stm32/f0/STM32CubeF0/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_dma.c
    ${CMAKE_SOURCE_DIR}/platform/st/stm32/f0/clock/clock.cpp
    ${CMAKE_SOURCE_DIR}/subsystem/sysclock/sysclock.cpp
)
target_include_directories(${PROJECT_NAME} PUBLIC 
    ${CMAKE_SOURCE_DIR}/board/
    ${CMAKE_SOURCE_DIR}/subsystem/
    ${CMAKE_SOURCE_DIR}/platform/st/stm32/f0/clock/
    ${CMAKE_SOURCE_DIR}/platform/st/stm32/f0/STM32CubeF0/Drivers/CMSIS/Device/ST/STM32F0xx/Include/
    ${CMAKE_SOURCE_DIR}/platform/st/stm32/f0/STM32CubeF0/Drivers/CMSIS/Core/Include/
    ${CMAKE_SOURCE_DIR}/platform/st/stm32/f0/STM32CubeF0/Drivers/STM32F0xx_HAL_Driver/Inc/
    ${CMAKE_SOURCE_DIR}/platform/st/stm32/f0/STM32CubeF0/Drivers/STM32F0xx_HAL_Driver/Inc/Legacy/
    ${CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES}
    ${CMAKE_C_STANDARD_INCLUDE_DIRECTORIES}
)

add_custom_command(OUTPUT ${PROJECT_NAME}.hex
    DEPENDS ${PROJECT_NAME}
    COMMAND ${CMAKE_OBJCOPY} -Oihex ${PROJECT_NAME} ${PROJECT_NAME}.hex
)

add_custom_command(OUTPUT ${PROJECT_NAME}.bin
    DEPENDS ${PROJECT_NAME}
    COMMAND ${CMAKE_OBJCOPY} -Obinary ${PROJECT_NAME} ${PROJECT_NAME}.bin
)

add_custom_command(OUTPUT ${PROJECT_NAME}.lst
    DEPENDS ${PROJECT_NAME}
    COMMAND ${CMAKE_OBJDUMP} -S ${PROJECT_NAME} > ${PROJECT_NAME}.lst
)

add_custom_target(hex ALL
    DEPENDS ${PROJECT_NAME}.hex
)

add_custom_target(bin ALL
    DEPENDS ${PROJECT_NAME}.bin
)

add_custom_target(lst ALL
    DEPENDS ${PROJECT_NAME}.lst
)
