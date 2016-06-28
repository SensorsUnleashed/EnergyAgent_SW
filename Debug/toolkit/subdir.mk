################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../toolkit/fastarithmetic.c 

S_UPPER_SRCS += \
../toolkit/isqrt64.S \
../toolkit/mac64_16_24.S \
../toolkit/mul48u_32_16.S \
../toolkit/mul64i_48_32.S \
../toolkit/mul64u_32_32.S 

OBJS += \
./toolkit/fastarithmetic.o \
./toolkit/isqrt64.o \
./toolkit/mac64_16_24.o \
./toolkit/mul48u_32_16.o \
./toolkit/mul64i_48_32.o \
./toolkit/mul64u_32_32.o 

C_DEPS += \
./toolkit/fastarithmetic.d 


# Each subdirectory must supply rules for building sources it contributes
toolkit/%.o: ../toolkit/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	msp430-elf-gcc -std=c99 -D__MSP430i2041__ -D_GNU_ASSEMBLER_ -I/home/omn/ti/gcc/include -I/home/omn/ti/gcc/msp430-elf/include -O0 -g3 -gdwarf-2 -ggdb -Wall -c -fmessage-length=0 -mmcu=msp430i2041 -fno-builtin-printf -fno-builtin-puts -fno-builtin-putc -Winline -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

toolkit/%.o: ../toolkit/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Assembler'
	msp430-elf-as -mmcu=msp430i2041 -Wall -g -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


