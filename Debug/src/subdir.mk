################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Delay.c \
../src/GPIO.c \
../src/LCD.c \
../src/LCDcounter.c \
../src/LED.c \
../src/SysTick.c \
../src/clib.c \
../src/cr_startup_lpc175x_6x.c \
../src/crp.c \
../src/datetime.c \
../src/input.c \
../src/mode_clock.c \
../src/tactSW.c 

C_DEPS += \
./src/Delay.d \
./src/GPIO.d \
./src/LCD.d \
./src/LCDcounter.d \
./src/LED.d \
./src/SysTick.d \
./src/clib.d \
./src/cr_startup_lpc175x_6x.d \
./src/crp.d \
./src/datetime.d \
./src/input.d \
./src/mode_clock.d \
./src/tactSW.d 

OBJS += \
./src/Delay.o \
./src/GPIO.o \
./src/LCD.o \
./src/LCDcounter.o \
./src/LED.o \
./src/SysTick.o \
./src/clib.o \
./src/cr_startup_lpc175x_6x.o \
./src/crp.o \
./src/datetime.o \
./src/input.o \
./src/mode_clock.o \
./src/tactSW.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -D__CODE_RED -DCORE_M3 -D__USE_CMSIS=CMSIS_CORE_LPC17xx -D__LPC17XX__ -D__REDLIB__ -I"C:\Users\4104720\Documents\MCUXpressoIDE_25.6.136\workspace\Degitalwatch\inc" -I"C:\Users\4104720\Documents\MCUXpressoIDE_25.6.136\20260416GitembeddedC1\20260416GitembeddedC1\embeddedC1\CMSIS_CORE_LPC17xx\inc" -O0 -fno-common -g3 -gdwarf-4 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m3 -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/Delay.d ./src/Delay.o ./src/GPIO.d ./src/GPIO.o ./src/LCD.d ./src/LCD.o ./src/LCDcounter.d ./src/LCDcounter.o ./src/LED.d ./src/LED.o ./src/SysTick.d ./src/SysTick.o ./src/clib.d ./src/clib.o ./src/cr_startup_lpc175x_6x.d ./src/cr_startup_lpc175x_6x.o ./src/crp.d ./src/crp.o ./src/datetime.d ./src/datetime.o ./src/input.d ./src/input.o ./src/mode_clock.d ./src/mode_clock.o ./src/tactSW.d ./src/tactSW.o

.PHONY: clean-src

