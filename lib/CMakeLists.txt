cmake_minimum_required(VERSION 3.16)

# ==============================================================================
# CMakeLists.txt for Embedded Robotics Shared Libraries
# Target MCU: STM32F446RE (ARM Cortex-M4F @ 180 MHz)
# ==============================================================================

set(STM32_LIBRARY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/STM32-Library")
set(STM32_COMPONENTS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/STM32-Components")
set(STM32_ALGORITHMS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Embedded-Algorithms")

set(CMSIS_DIR "${STM32_LIBRARY_DIR}/Drivers/CMSIS")

# ------------------------------------------------------------------------------
# Layer 1: Hardware Abstraction & Peripheral Engine (stm32::hal)
# ------------------------------------------------------------------------------
set(STM32_HAL_SOURCES
    ${STM32_LIBRARY_DIR}/Src/syscalls.c
    ${STM32_LIBRARY_DIR}/Src/sysmem.c
    ${CMSIS_DIR}/Device/ST/STM32F4xx/Source/Templates/system_stm32f4xx.c
    ${STM32_LIBRARY_DIR}/Startup/startup_stm32f446retx.s
    ${STM32_LIBRARY_DIR}/Inc/stm32/common/systick/systick.cpp
    ${STM32_LIBRARY_DIR}/Inc/stm32/gpio/digital_input/digital_input.cpp
    ${STM32_LIBRARY_DIR}/Inc/stm32/gpio/digital_output/digital_output.cpp
    ${STM32_LIBRARY_DIR}/Inc/stm32/gpio/interrupt_input/interrupt_input.cpp
    ${STM32_LIBRARY_DIR}/Inc/stm32/exti/exti_manager.cpp
    ${STM32_LIBRARY_DIR}/Inc/stm32/exti/exti_irq.cpp
    ${STM32_LIBRARY_DIR}/Inc/stm32/timer/interrupt/timer_interrupt.cpp
    ${STM32_LIBRARY_DIR}/Inc/stm32/timer/interrupt/timer_irq.cpp
    ${STM32_LIBRARY_DIR}/Inc/stm32/uart/interrupt/uart_interrupt.cpp
    ${STM32_LIBRARY_DIR}/Inc/stm32/uart/interrupt/uart_irq.cpp
    ${STM32_LIBRARY_DIR}/Inc/stm32/spi/interrupt/spi_interrupt.cpp
    ${STM32_LIBRARY_DIR}/Inc/stm32/spi/interrupt/spi_irq.cpp
    ${STM32_LIBRARY_DIR}/Inc/stm32/i2c/interrupt/i2c_interrupt.cpp
    ${STM32_LIBRARY_DIR}/Inc/stm32/i2c/interrupt/i2c_irq.cpp
    ${STM32_LIBRARY_DIR}/Inc/stm32/adc/interrupt/adc_interrupt.cpp
    ${STM32_LIBRARY_DIR}/Inc/stm32/adc/interrupt/adc_irq.cpp
    ${STM32_LIBRARY_DIR}/Inc/stm32/dma/interrupt/dma_interrupt.cpp
    ${STM32_LIBRARY_DIR}/Inc/stm32/dma/interrupt/dma_irq.cpp
)

add_library(stm32_hal STATIC ${STM32_HAL_SOURCES})
add_library(stm32::hal ALIAS stm32_hal)
add_library(stm32_core ALIAS stm32_hal)

target_include_directories(stm32_hal PUBLIC
    ${STM32_LIBRARY_DIR}/Inc
    ${CMSIS_DIR}/Device/ST/STM32F4xx/Include
    ${CMSIS_DIR}/Include
)

target_compile_definitions(stm32_hal PUBLIC STM32F446xx)

target_link_options(stm32_hal INTERFACE
    -mcpu=cortex-m4
    -mthumb
    -mfpu=fpv4-sp-d16
    -mfloat-abi=hard
    -T${STM32_LIBRARY_DIR}/STM32F446RETX_FLASH.ld
    -specs=nano.specs
    -specs=nosys.specs
    -Wl,--gc-sections
)

# ------------------------------------------------------------------------------
# Layer 2: External Device Drivers (stm32::components)
# ------------------------------------------------------------------------------
add_library(stm32_components INTERFACE)
add_library(stm32::components ALIAS stm32_components)

target_include_directories(stm32_components INTERFACE
    ${STM32_COMPONENTS_DIR}/include
)

target_link_libraries(stm32_components INTERFACE
    stm32::hal
)

# ------------------------------------------------------------------------------
# Layer 3 / Algorithms: Embedded Algorithms (stm32::algorithms)
# ------------------------------------------------------------------------------
if(EXISTS "${STM32_ALGORITHMS_DIR}/include")
    add_library(stm32_algorithms INTERFACE)
    add_library(stm32::algorithms ALIAS stm32_algorithms)

    target_include_directories(stm32_algorithms INTERFACE
        ${STM32_ALGORITHMS_DIR}/include
    )
endif()
