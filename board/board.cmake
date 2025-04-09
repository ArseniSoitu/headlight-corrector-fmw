set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

IF(NOT TARGET_TRIPLET)
    SET(TARGET_TRIPLET "arm-none-eabi")
    MESSAGE(STATUS "No TARGET_TRIPLET specified, using default: " ${TARGET_TRIPLET})
ENDIF()

SET(CMAKE_SYSTEM_NAME Generic CACHE INTERNAL "system name")
SET(CMAKE_SYSTEM_PROCESSOR arm CACHE INTERNAL "processor")

SET(CMAKE_C_COMPILER ${TARGET_TRIPLET}-gcc CACHE INTERNAL "c compiler")
SET(CMAKE_CXX_COMPILER ${TARGET_TRIPLET}-g++ CACHE INTERNAL "cxx compiler")
SET(CMAKE_ASM_COMPILER ${TARGET_TRIPLET}-as CACHE INTERNAL "asm compiler")

SET(CMAKE_OBJCOPY ${TARGET_TRIPLET}-objcopy CACHE INTERNAL "objcopy")
SET(CMAKE_OBJDUMP ${TARGET_TRIPLET}-objdump CACHE INTERNAL "objdump")

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

SET(CMAKE_EXE_LINKER_FLAGS "-nostartfiles -T ${CMAKE_SOURCE_DIR}/board/ld/stm32f0.ld --specs=nosys.specs --specs=nano.specs -Wl,--gc-sections -mthumb -mcpu=${CPU_TYPE}" CACHE INTERNAL "exe link flags")
