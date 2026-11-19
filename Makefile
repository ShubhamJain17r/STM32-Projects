# ==============================================================================
# Makefile for STM32-Projects: Incubator & Multi-Robot Template Playground
# Target MCU: STM32F446RE (ARM Cortex-M4F @ 180 MHz)
# ==============================================================================

# User-selectable target (defaults to primary benchmark: apps/line_follower_benchmark)
DEFAULT_APP ?= apps/line_follower_benchmark

APP ?= $(DEFAULT_APP)

# Normalize APP: allow both APP=apps/<target> and APP=<target>
ifeq ($(wildcard $(APP)),)
    ifneq ($(wildcard apps/$(APP)),)
        APP_PATH := apps/$(APP)
    else
        APP_PATH := $(APP)
    endif
else
    APP_PATH := $(APP)
endif

# Toolchain configuration (override via environment or CLI: make ARM_TOOLCHAIN_DIR=...)
ifneq ($(ARM_TOOLCHAIN_DIR),)
    PREFIX = $(ARM_TOOLCHAIN_DIR)/arm-none-eabi-
else ifneq ($(wildcard /home/shubham/.arduino15/packages/arduino/tools/arm-none-eabi-gcc/7-2017q4/bin/arm-none-eabi-gcc),)
    PREFIX = /home/shubham/.arduino15/packages/arduino/tools/arm-none-eabi-gcc/7-2017q4/bin/arm-none-eabi-
else
    PREFIX = arm-none-eabi-
endif

export PREFIX

.PHONY: all clean clean-all size flash flash-stlink help test-submodules

all:
	@if [ -d "$(APP_PATH)" ]; then \
		echo "======================================================================"; \
		echo "Building Application Prototype: $(APP_PATH)"; \
		echo "======================================================================"; \
		$(MAKE) -C $(APP_PATH) all; \
	else \
		echo "Error: Target directory '$(APP_PATH)' not found."; \
		echo "Specify a valid app using APP=apps/<target> or APP=<target>."; \
		exit 1; \
	fi

clean:
	@if [ -d "$(APP_PATH)" ]; then \
		echo "Cleaning build artifacts in $(APP_PATH)..."; \
		$(MAKE) -C $(APP_PATH) clean; \
	else \
		echo "Target directory '$(APP_PATH)' not found."; \
	fi

clean-all:
	@echo "Cleaning all application prototypes in apps/..."
	@for app in apps/*; do \
		if [ -f "$$app/Makefile" ]; then \
			echo "Cleaning $$app..."; \
			$(MAKE) -C "$$app" clean; \
		fi \
	done

size:
	@if [ -d "$(APP_PATH)" ]; then \
		$(MAKE) -C $(APP_PATH) size; \
	else \
		echo "Target directory '$(APP_PATH)' not found."; \
	fi

flash:
	@if [ -d "$(APP_PATH)" ]; then \
		$(MAKE) -C $(APP_PATH) flash; \
	else \
		echo "Target directory '$(APP_PATH)' not found."; \
	fi

flash-stlink:
	@if [ -d "$(APP_PATH)" ]; then \
		$(MAKE) -C $(APP_PATH) flash-stlink; \
	else \
		echo "Target directory '$(APP_PATH)' not found."; \
	fi

test-submodules:
	@echo "Testing Layer 1 (STM32-Library) build..."
	@$(MAKE) -C lib/STM32-Library all
	@echo "Testing Layer 2 (STM32-Components) build..."
	@$(MAKE) -C lib/STM32-Components EXAMPLE=examples/smoke_test LIB_DIR=../STM32-Library all
	@echo "All submodules built successfully!"

help:
	@echo "STM32-Projects: Incubator & Multi-Robot Template Playground"
	@echo "==========================================================="
	@echo "make                             : Build default benchmark ($(DEFAULT_APP))"
	@echo "make APP=apps/<target>           : Build specific application (e.g. apps/template_project)"
	@echo "make APP=<target>                : Build specific application by name (e.g. template_project)"
	@echo "make clean                       : Clean build directory for current target"
	@echo "make clean-all                   : Clean all application prototypes in apps/"
	@echo "make size                        : Print binary memory footprint"
	@echo "make flash                       : Program MCU via OpenOCD"
	@echo "make flash-stlink                : Program MCU via ST-Link utility"
	@echo "make test-submodules             : Validate builds of linked submodules"
