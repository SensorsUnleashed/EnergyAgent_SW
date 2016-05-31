################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../toolkit/fastarithmetic.c 

OBJS += \
./toolkit/fastarithmetic.o 

C_DEPS += \
./toolkit/fastarithmetic.d 


# Each subdirectory must supply rules for building sources it contributes
toolkit/%.o: ../toolkit/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	msp430-elf-gcc -std=c99 -D__MSP430i2041__ -I/home/omn/ti/gcc/include -I/home/omn/ti/gcc/msp430-elf/include -O0 -g3 -gdwarf-2 -ggdb -Wall -c -fmessage-length=0 -mmcu=msp430i2041 -fno-builtin-printf -fno-builtin-puts -fno-builtin-putc -Winline -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


