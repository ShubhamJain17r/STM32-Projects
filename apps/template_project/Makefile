# ==============================================================================
# Makefile for Robot Starter Template Application (apps/template_project)
# Target MCU: STM32F446RE (ARM Cortex-M4F @ 180 MHz)
# ==============================================================================

TARGET    = template_project
BUILD_DIR = build

# Base paths to submodules
ROOT_DIR  = ../..
COMP_DIR  = $(ROOT_DIR)/lib/STM32-Components
ALGO_DIR  = $(ROOT_DIR)/lib/Embedded-Algorithms

# Resolve Layer 1 directory (prefer components nested submodule, fallback to root submodule)
ifneq ($(wildcard $(COMP_DIR)/lib/STM32-Library/Inc),)
    LIB_DIR = $(COMP_DIR)/lib/STM32-Library
else
    LIB_DIR = $(ROOT_DIR)/lib/STM32-Library
endif

CMSIS_DIR = $(LIB_DIR)/Drivers/CMSIS

# Toolchain configuration (override via environment or CLI: make ARM_TOOLCHAIN_DIR=...)
ifneq ($(ARM_TOOLCHAIN_DIR),)
    PREFIX = $(ARM_TOOLCHAIN_DIR)/arm-none-eabi-
else ifneq ($(wildcard /home/shubham/.arduino15/packages/arduino/tools/arm-none-eabi-gcc/7-2017q4/bin/arm-none-eabi-gcc),)
    PREFIX = /home/shubham/.arduino15/packages/arduino/tools/arm-none-eabi-gcc/7-2017q4/bin/arm-none-eabi-
else
    PREFIX = arm-none-eabi-
endif

CC      = $(PREFIX)gcc
CXX     = $(PREFIX)g++
AS      = $(PREFIX)gcc -x assembler-with-cpp
CP      = $(PREFIX)objcopy
SZ      = $(PREFIX)size
HEX     = $(CP) -O ihex
BIN     = $(CP) -O binary -S

# Target Architecture Flags (STM32F446RE Cortex-M4F)
MCU = -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard

# Hardware Defines
DEFS = -DSTM32F446xx

# Include Paths (Application + Layer 2 Components + Layer 1 Peripheral Engine + CMSIS + Algorithms)
INCLUDES = \
    -Iinclude \
    -I$(COMP_DIR)/include \
    -I$(LIB_DIR)/Inc \
    -I$(CMSIS_DIR)/Device/ST/STM32F4xx/Include \
    -I$(CMSIS_DIR)/Include

ifneq ($(wildcard $(ALGO_DIR)/include),)
    INCLUDES += -I$(ALGO_DIR)/include
endif

# Compiler Optimization & Strict Warning Flags
COMMON_FLAGS = $(MCU) $(DEFS) $(INCLUDES) -Wall -Wextra -Wpedantic -Werror -ffunction-sections -fdata-sections -O2 -g3
CFLAGS   = $(COMMON_FLAGS) -std=c11
CXXFLAGS = $(COMMON_FLAGS) -std=c++17 -fno-exceptions -fno-rtti -fno-use-cxa-atexit
ASFLAGS  = $(MCU) $(DEFS) $(INCLUDES) -Wall -fdata-sections -ffunction-sections

# Linker Configuration
LDSCRIPT = $(LIB_DIR)/STM32F446RETX_FLASH.ld
LDFLAGS  = $(MCU) -T$(LDSCRIPT) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections -specs=nano.specs -specs=nosys.specs

# Layer 1 Sources
LIB_C_SOURCES = \
    $(LIB_DIR)/Src/syscalls.c \
    $(LIB_DIR)/Src/sysmem.c \
    $(CMSIS_DIR)/Device/ST/STM32F4xx/Source/Templates/system_stm32f4xx.c

LIB_CPP_SOURCES = \
    $(LIB_DIR)/Inc/stm32/common/systick/systick.cpp \
    $(LIB_DIR)/Inc/stm32/gpio/digital_input/digital_input.cpp \
    $(LIB_DIR)/Inc/stm32/gpio/digital_output/digital_output.cpp \
    $(LIB_DIR)/Inc/stm32/gpio/interrupt_input/interrupt_input.cpp \
    $(LIB_DIR)/Inc/stm32/exti/exti_manager.cpp \
    $(LIB_DIR)/Inc/stm32/exti/exti_irq.cpp \
    $(LIB_DIR)/Inc/stm32/timer/interrupt/timer_interrupt.cpp \
    $(LIB_DIR)/Inc/stm32/timer/interrupt/timer_irq.cpp \
    $(LIB_DIR)/Inc/stm32/uart/interrupt/uart_interrupt.cpp \
    $(LIB_DIR)/Inc/stm32/uart/interrupt/uart_irq.cpp \
    $(LIB_DIR)/Inc/stm32/spi/interrupt/spi_interrupt.cpp \
    $(LIB_DIR)/Inc/stm32/spi/interrupt/spi_irq.cpp \
    $(LIB_DIR)/Inc/stm32/i2c/interrupt/i2c_interrupt.cpp \
    $(LIB_DIR)/Inc/stm32/i2c/interrupt/i2c_irq.cpp \
    $(LIB_DIR)/Inc/stm32/adc/interrupt/adc_interrupt.cpp \
    $(LIB_DIR)/Inc/stm32/adc/interrupt/adc_irq.cpp \
    $(LIB_DIR)/Inc/stm32/dma/interrupt/dma_interrupt.cpp \
    $(LIB_DIR)/Inc/stm32/dma/interrupt/dma_irq.cpp

LIB_ASM_SOURCES = \
    $(LIB_DIR)/Startup/startup_stm32f446retx.s

# Application Sources
APP_CPP_SOURCES = $(wildcard src/*.cpp)
APP_C_SOURCES   = $(wildcard src/*.c)

# Aggregate sources
ALL_C_SOURCES   = $(LIB_C_SOURCES) $(APP_C_SOURCES)
ALL_CPP_SOURCES = $(LIB_CPP_SOURCES) $(APP_CPP_SOURCES)
ALL_ASM_SOURCES = $(LIB_ASM_SOURCES)

# Generate Object File List
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(ALL_C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(ALL_C_SOURCES)))

OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ALL_CPP_SOURCES:.cpp=.o)))
vpath %.cpp $(sort $(dir $(ALL_CPP_SOURCES)))

OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ALL_ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ALL_ASM_SOURCES)))

# ==============================================================================
# Build Targets
# ==============================================================================

.PHONY: all clean size flash flash-stlink help

all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin size

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	@echo "  CC    $<"
	@$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.cpp | $(BUILD_DIR)
	@echo "  CXX   $<"
	@$(CXX) -c $(CXXFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.cpp=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.s | $(BUILD_DIR)
	@echo "  AS    $<"
	@$(AS) -c $(ASFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) | $(BUILD_DIR)
	@echo "  LINK  $@"
	@$(CXX) $(OBJECTS) $(LDFLAGS) -o $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	@echo "  HEX   $@"
	@$(HEX) $< $@

$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	@echo "  BIN   $@"
	@$(BIN) $< $@

$(BUILD_DIR):
	@mkdir -p $@

size: $(BUILD_DIR)/$(TARGET).elf
	@echo "  SIZE  $<"
	@$(SZ) --format=berkeley $<

clean:
	@echo "Cleaning $(TARGET) build artifacts..."
	@rm -rf $(BUILD_DIR)

flash: all
	openocd -f board/st_nucleo_f4.cfg -c "program $(BUILD_DIR)/$(TARGET).elf verify reset exit"

flash-stlink: all
	st-flash write $(BUILD_DIR)/$(TARGET).bin 0x08000000

help:
	@echo "Robot Starter Template Build System"
	@echo "-----------------------------------"
	@echo "make              : Build firmware binary"
	@echo "make size         : Show memory footprint"
	@echo "make clean        : Remove build directory"
	@echo "make flash        : Flash via OpenOCD"
	@echo "make flash-stlink : Flash via st-flash"
