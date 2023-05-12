################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Startup/subdir.mk
-include Src/subdir.mk
-include Kalman_filter/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := BT-TH
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
BT-TH.elf \

MAP_FILES += \
BT-TH.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
BT-TH.list \


# All Target
all: main-build

# Main-build Target
main-build: BT-TH.elf secondary-outputs

# Tool invocations
BT-TH.elf BT-TH.map: $(OBJS) $(USER_OBJS) C:\Users\ASUS\STM32CubeIDE\workspace_1.11.2\BT-TH\STM32F401RETX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "BT-TH.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"C:\Users\ASUS\STM32CubeIDE\workspace_1.11.2\BT-TH\STM32F401RETX_FLASH.ld" --specs=nosys.specs -Wl,-Map="BT-TH.map" -Wl,--gc-sections -static -L"C:\Users\ASUS\Desktop\Downloads\SDK_1.0.3_NUCLEO-F401RE\lib_stm" -L"C:\Users\ASUS\Desktop\Downloads\Kalman_filter" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

BT-TH.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "BT-TH.list"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) BT-TH.elf BT-TH.list BT-TH.map default.size.stdout
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets
