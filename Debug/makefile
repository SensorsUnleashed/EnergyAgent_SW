################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include driverlib/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: EnergyAgent-1Ch.elf

# Tool invocations
EnergyAgent-1Ch.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross GCC Linker'
	msp430-elf-gcc -L/home/omn/ti/gcc/include -mmcu=msp430i2041 -v -Wl,-Map,EnergyAgent-1Ch.map -o "EnergyAgent-1Ch.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) EnergyAgent-1Ch.elf
	-@echo ' '

post-build:
	-@echo 'Generate Hex'
	-objcopy -O ihex EnergyAgent-1Ch.elf EnergyAgent-1Ch.hex
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
