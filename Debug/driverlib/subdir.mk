################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../driverlib/cs.c \
../driverlib/eusci_a_spi.c \
../driverlib/eusci_a_uart.c \
../driverlib/eusci_b_i2c.c \
../driverlib/eusci_b_spi.c \
../driverlib/flashctl.c \
../driverlib/gpio.c \
../driverlib/mpy.c \
../driverlib/pmm.c \
../driverlib/sd24.c \
../driverlib/sfr.c \
../driverlib/timer_a.c \
../driverlib/tlv.c \
../driverlib/wdt.c 

OBJS += \
./driverlib/cs.o \
./driverlib/eusci_a_spi.o \
./driverlib/eusci_a_uart.o \
./driverlib/eusci_b_i2c.o \
./driverlib/eusci_b_spi.o \
./driverlib/flashctl.o \
./driverlib/gpio.o \
./driverlib/mpy.o \
./driverlib/pmm.o \
./driverlib/sd24.o \
./driverlib/sfr.o \
./driverlib/timer_a.o \
./driverlib/tlv.o \
./driverlib/wdt.o 

C_DEPS += \
./driverlib/cs.d \
./driverlib/eusci_a_spi.d \
./driverlib/eusci_a_uart.d \
./driverlib/eusci_b_i2c.d \
./driverlib/eusci_b_spi.d \
./driverlib/flashctl.d \
./driverlib/gpio.d \
./driverlib/mpy.d \
./driverlib/pmm.d \
./driverlib/sd24.d \
./driverlib/sfr.d \
./driverlib/timer_a.d \
./driverlib/tlv.d \
./driverlib/wdt.d 


# Each subdirectory must supply rules for building sources it contributes
driverlib/%.o: ../driverlib/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	msp430-elf-gcc -std=c99 -D__MSP430i2041__ -I/home/omn/ti/gcc/include -I/home/omn/ti/gcc/msp430-elf/include -O0 -g3 -gdwarf-2 -ggdb -Wall -c -fmessage-length=0 -mmcu=msp430i2041 -fno-builtin-printf -fno-builtin-puts -fno-builtin-putc -Winline -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


