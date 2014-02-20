################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
O_SRCS += \
../usermods/example.o 

C_SRCS += \
../usermods/example.c \
../usermods/motores.c \
../usermods/myDelay.c \
../usermods/robot.c \
../usermods/sensorIR.c \
../usermods/uart.c \
../usermods/ultrasom.c \
../usermods/ultrasom_stepper.c 

OBJS += \
./usermods/example.o \
./usermods/motores.o \
./usermods/myDelay.o \
./usermods/robot.o \
./usermods/sensorIR.o \
./usermods/uart.o \
./usermods/ultrasom.o \
./usermods/ultrasom_stepper.o 

C_DEPS += \
./usermods/example.d \
./usermods/motores.d \
./usermods/myDelay.d \
./usermods/robot.d \
./usermods/sensorIR.d \
./usermods/uart.d \
./usermods/ultrasom.d \
./usermods/ultrasom_stepper.d 


# Each subdirectory must supply rules for building sources it contributes
usermods/%.o: ../usermods/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -I/home/victor/workspace/robot_all_in_c/robot_aic/src -Wall -O2 -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega328p -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


