cmake_minimum_required(VERSION 3.16)

# ==============================================================================
# CMakeLists.txt for STM32-Projects: Incubator & Multi-Robot Template Playground
# Target MCU: STM32F446RE (ARM Cortex-M4F @ 180 MHz)
# ==============================================================================

# Set ARM GCC toolchain file if not already specified
if(NOT CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/lib/STM32-Library/cmake/arm-none-eabi-gcc.cmake")
endif()

project(STM32-Projects LANGUAGES C CXX ASM)

# Embedded Standards
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Hardware Definitions
add_compile_definitions(STM32F446xx)

# Architecture & Optimization Flags (STM32F446RE: Cortex-M4 with single-precision FPU)
set(MCU_FLAGS "-mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${MCU_FLAGS} -Wall -Wextra -Wpedantic -O2 -ffunction-sections -fdata-sections")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MCU_FLAGS} -Wall -Wextra -Wpedantic -O2 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit")
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} ${MCU_FLAGS}")

# ------------------------------------------------------------------------------
# 1. Add Shared Libraries (Layer 1 HAL, Layer 2 Components, Algorithms)
# ------------------------------------------------------------------------------
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/lib/CMakeLists.txt")
    add_subdirectory(lib)
endif()

# ------------------------------------------------------------------------------
# 2. Dynamically Discover and Add Application Prototypes from apps/
# ------------------------------------------------------------------------------
file(GLOB APP_ENTRIES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/apps" "${CMAKE_CURRENT_SOURCE_DIR}/apps/*")

foreach(ENTRY ${APP_ENTRIES})
    if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/apps/${ENTRY}" AND
       EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/apps/${ENTRY}/CMakeLists.txt")
        message(STATUS "Configuring robot application prototype: apps/${ENTRY}")
        add_subdirectory("apps/${ENTRY}")
    endif()
endforeach()
