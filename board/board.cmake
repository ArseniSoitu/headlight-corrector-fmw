set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)


if(NOT DEFINED TOOLCHAIN_PREFIX)
    set(TOOLCHAIN_PREFIX "arm-none-eabi")
    message(STATUS "TOOLCHAIN_PREFIX not specified, using: " ${TOOLCHAIN_PREFIX})
endif()

find_program(CMAKE_C_COMPILER NAMES ${TOOLCHAIN_PREFIX}-gcc HINTS ${TOOLCHAIN_BIN_PATH})
find_program(CMAKE_CXX_COMPILER NAMES ${TOOLCHAIN_PREFIX}-g++ HINTS ${TOOLCHAIN_BIN_PATH})
find_program(CMAKE_ASM_COMPILER NAMES ${TOOLCHAIN_PREFIX}-gcc HINTS ${TOOLCHAIN_BIN_PATH})
find_program(CMAKE_OBJCOPY NAMES ${TOOLCHAIN_PREFIX}-objcopy HINTS ${TOOLCHAIN_BIN_PATH})
find_program(CMAKE_OBJDUMP NAMES ${TOOLCHAIN_PREFIX}-objdump HINTS ${TOOLCHAIN_BIN_PATH})

# Select cpu type.
SET(CPU_TYPE cortex-m0)

SET(CMAKE_C_FLAGS "-DSTM32F070xB -DUSE_FULL_LL_DRIVER -mthumb -mcpu=${CPU_TYPE} -fno-builtin -Wall -std=gnu99 -fdata-sections -ffunction-sections" CACHE INTERNAL "c compiler flags")
SET(CMAKE_CXX_FLAGS "-DSTM32F070xB -DUSE_FULL_LL_DRIVER -mthumb -mcpu=${CPU_TYPE} -fno-builtin -Wall -std=c++17  -fdata-sections -ffunction-sections" CACHE INTERNAL "cxx compiler flags")
SET(CMAKE_ASM_FLAGS "-mthumb -mcpu=${CPU_TYPE}" CACHE INTERNAL "asm compiler flags")

SET(CMAKE_C_FLAGS_DEBUG "-O0 -g -gstabs+" CACHE INTERNAL "c debug compiler flags")
SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -gstabs+" CACHE INTERNAL "cxx debug compiler flags")
SET(CMAKE_ASM_FLAGS_DEBUG "-g -gstabs+" CACHE INTERNAL "asm debug compiler flags")

SET(CMAKE_C_FLAGS_RELEASE "-Os" CACHE INTERNAL "c release compiler flags")
SET(CMAKE_CXX_FLAGS_RELEASE "-Os" CACHE INTERNAL "cxx release compiler flags")
SET(CMAKE_ASM_FLAGS_RELEASE "" CACHE INTERNAL "asm release compiler flags")

SET(CMAKE_EXE_LINKER_FLAGS "-nostartfiles -Tstm32f0.ld -L${CMAKE_SOURCE_DIR}/board/ld/ --specs=nosys.specs --specs=nano.specs -Wl,--gc-sections -mthumb -mcpu=${CPU_TYPE}"  CACHE INTERNAL "exe link flags")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
