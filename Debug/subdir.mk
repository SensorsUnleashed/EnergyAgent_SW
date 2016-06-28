################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../communication.c \
../main.c \
../metrology-background.c \
../printf.c 

OBJS += \
./communication.o \
./main.o \
./metrology-background.o \
./printf.o 

C_DEPS += \
./communication.d \
./main.d \
./metrology-background.d \
./printf.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	msp430-elf-gcc -std=c99 -D__MSP430i2041__ -D_GNU_ASSEMBLER_ -I/home/omn/ti/gcc/include -I/home/omn/ti/gcc/msp430-elf/include -O0 -g3 -gdwarf-2 -ggdb -Wall -c -fmessage-length=0 -mmcu=msp430i2041 -fno-builtin-printf -fno-builtin-puts -fno-builtin-putc -Winline -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


