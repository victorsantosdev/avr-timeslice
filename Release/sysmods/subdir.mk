################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
O_SRCS += \
../sysmods/adc.o \
../sysmods/digitalrw.o \
../sysmods/led.o \
../sysmods/list.o \
../sysmods/pwm.o \
../sysmods/time.o \
../sysmods/uart.o 

C_SRCS += \
../sysmods/adc.c \
../sysmods/digitalrw.c \
../sysmods/led.c \
../sysmods/list.c \
../sysmods/pwm.c \
../sysmods/time.c \
../sysmods/uart.c 

OBJS += \
./sysmods/adc.o \
./sysmods/digitalrw.o \
./sysmods/led.o \
./sysmods/list.o \
./sysmods/pwm.o \
./sysmods/time.o \
./sysmods/uart.o 

C_DEPS += \
./sysmods/adc.d \
./sysmods/digitalrw.d \
./sysmods/led.d \
./sysmods/list.d \
./sysmods/pwm.d \
./sysmods/time.d \
./sysmods/uart.d 


# Each subdirectory must supply rules for building sources it contributes
sysmods/%.o: ../sysmods/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -I/home/victor/workspace/robot_all_in_c/robot_aic/src -Wall -O2 -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega328p -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


