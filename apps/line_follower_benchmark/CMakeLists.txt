cmake_minimum_required(VERSION 3.16)

# ==============================================================================
# CMakeLists.txt for High-Speed Closed-Loop Line Follower Benchmark
# Target: STM32F446RE (ARM Cortex-M4F @ 180 MHz)
# ==============================================================================

set(APP_TARGET line_follower_benchmark)

# Application Sources
file(GLOB_RECURSE APP_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
)

add_executable(${APP_TARGET}.elf ${APP_SOURCES})

target_include_directories(${APP_TARGET}.elf PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# Consume modern CMake targets from lib/
target_link_libraries(${APP_TARGET}.elf PRIVATE
    stm32::components
)

if(TARGET stm32::algorithms)
    target_link_libraries(${APP_TARGET}.elf PRIVATE
        stm32::algorithms
    )
endif()

target_link_options(${APP_TARGET}.elf PRIVATE
    -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/${APP_TARGET}.map,--cref
)

add_custom_command(TARGET ${APP_TARGET}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary ${APP_TARGET}.elf ${APP_TARGET}.bin
    COMMAND ${CMAKE_OBJCOPY} -O ihex   ${APP_TARGET}.elf ${APP_TARGET}.hex
    COMMAND ${CMAKE_SIZE} --format=berkeley ${APP_TARGET}.elf
    COMMENT "Building raw flash binaries (.bin/.hex) and displaying memory footprint for ${APP_TARGET}:"
)
